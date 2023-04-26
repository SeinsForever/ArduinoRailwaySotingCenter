#include "WString.h"
#include "HardwareSerial.h"
#include "Arduino.h"
#include "Message.h"
using namespace MessageBase;

Message::Message(unsigned int workFreaquency, byte thisControllerNumber, byte typeOfThisController, byte controllerNumberWordLen = 2, byte deviceNumberWordLen = 4,
            byte busPin = 2, byte debug = 0)
{
  _workFreaquency = workFreaquency;
  _thisControllerNumber = thisControllerNumber;
  _debug = debug;
          if(_debug)
            Serial.println("\n--------------------\nMessage contructor");

  changeOfMessageProperties(controllerNumberWordLen, deviceNumberWordLen);

  _busPin = busPin;

          if(_debug)
            Serial.println("OUTPUT Pins:");
  for(int i = 0; i < sizeof(_outputDevicePin) / sizeof(_outputDevicePin[0]) ;i++)
  {
          if(_debug)
            Serial.println("\t-Pin " + String(i) + ": " + String(_outputDevicePin[i]));
    pinMode(i, OUTPUT);
  }
  
          if(_debug)
            Serial.println("INPUT Pins:");
  for(int i = 0; i < sizeof(_inputDevicePin) / sizeof(_inputDevicePin[0]) ;i++)
  {
          if(_debug)
            Serial.println("\t-Pin " + String(i) + ": " + String(_inputDevicePin[i]));
    pinMode(i, OUTPUT);
  }

  pinMode(_busPin, INPUT);

  switch (typeOfThisController) 
  {
    case(0): // light
      Serial.println("Type of this controller is light");
      _deviceAmount = 12;
      long pathDeviceLightMap[7] = {0b100000000000,
                                    0b011100000000,
                                    0b011010000000,
                                    0b010001110000,
                                    0b010001101000,
                                    0b010001000110,
                                    0b010001000101};  
      for(int i = 0; i < sizeof(pathDeviceLightMap) / sizeof(pathDeviceLightMap[0]); i++)
      {
        _pathDeviceLightMap[i] = pathDeviceLightMap[i];
        if(_debug)
        {
          Serial.println("-path " + String(i) + " = " + String(_pathDeviceLightMap[i], BIN));
        }
      }
      break;
    case(1): // arrows
      Serial.println("Type of this controller is arrows");
      _deviceAmount = 6;
      long pathDeviceArrowMap[7] = {0b100000,
                                    0b011000,
                                    0b001000,
                                    0b000000,
                                    0b000100,
                                    0b000010,
                                    0b000011};  
      for(int i = 0; i < sizeof(pathDeviceArrowMap) / sizeof(pathDeviceArrowMap[0]); i++)
      {
        _pathDeviceLightMap[i] = pathDeviceArrowMap[i];
      }
      break;
    default:
      Serial.println("Error! unknown controller type.");
      break;
  }
}

void Message::changeOfMessageProperties(byte newControllerNumberWordLen = NULL, byte newDeviceNumberWordLen = NULL)
{
  if(newControllerNumberWordLen)
    _controllerNumberWordLen = newControllerNumberWordLen;
  
  if(newDeviceNumberWordLen)
    _pathNumberWordLen = newDeviceNumberWordLen;
}

void Message::formingMessage(byte controllerNumber, byte deviceNumber)
{
  _sendingMessage = 0;

  _sendingMessage = 1;/// Start bit
  _sendingMessage = _sendingMessage << _controllerNumberWordLen + _pathNumberWordLen + 1; /// Start bit shift

  /// bit(_controllerNumberWordLen) == 2 ^ _controllerNumberWordLen => result - 1 = 0..01..1 with _controllerNumberWordLen units
  /// The previous result is a Mask to controllerNumber
  /// Then Mask shift by _pathNumberWordLen + 1 bits
  /// Last operation is _sendingMessage OR Mask (previous result)
  _sendingMessage = _sendingMessage | ((controllerNumber & bit(_controllerNumberWordLen) - 1) << _pathNumberWordLen + 1);

  _sendingMessage = _sendingMessage | ((deviceNumber & bit(_pathNumberWordLen) - 1) << 1);
  
}

void Message::printProperties()
{
  Serial.println("Properties:\n -Controller Numb WL: " + String(_controllerNumberWordLen) + "\n -Device WL: " + String(_pathNumberWordLen) + "\n -Word Length: " + String(_controllerNumberWordLen + _pathNumberWordLen + 1));
}

void Message::printMessage()
{
  Serial.println("Message:\n -BIN: " + String(_sendingMessage, BIN) + "\n -DEC: " + String(_sendingMessage));
}

void Message::timerSet(byte timerNumb, unsigned long time = millis())
{
  _timer[timerNumb] = time;
}

unsigned long Message::timerRead(byte timerNumb)
{
  return _timer[timerNumb];
}

void Message::sendMessageInPort()
{ 
  if(_sendingMessage > 0)
  { 
    pinMode(_busPin, OUTPUT);
    if(millis() - Message::timerRead(0) >= _workFreaquency)
    {

      Message::timerSet(0);

      if(!_processReceivingMessage)
      {
        if(_processSendingMessage == 0) 
        {
          _processSendingMessage = 1;
          _sendingMessageMask = bit(_controllerNumberWordLen + _pathNumberWordLen + 1);
                if(_debug)
                  Serial.println("Message is want to send!");
        }
        
                if(_debug)
                  Serial.println(" Message: " + String(_sendingMessage, BIN)+ " " + "Mask: " + String(_sendingMessageMask) 
                      + " " + String(_sendingMessageMask, BIN) + " &?: " 
                      + String((_sendingMessage & _sendingMessageMask) > 0));

        digitalWrite(_busPin, (_sendingMessage & _sendingMessageMask) > 0);
        _sendingMessageMask = _sendingMessageMask >> 1;

        if(_sendingMessageMask == 0)
        {
          _processSendingMessage = 0;
          _sendingMessage = 0;
                if(_debug)
                  Serial.println("Message has been successfully send!");
        }
      }
    }
  }
}


void Message::recieveMessageInPort()
{
  if(_processSendingMessage == false)
  {
    if(_processReceivingMessage == false)
    {
      pinMode(_busPin, INPUT);
      if(digitalRead(_busPin) == 1)
      {
              if(_debug)
                Serial.println("Start bit (1) has been recieved!");
        _recievedMessage = 1; // start bit
        _processReceivingMessage = true;
        Message::timerSet(0, millis());
      }
    }
    else if(_processReceivingMessage == true)
    {
      if(millis() - Message::timerRead(0) >= _workFreaquency)
      {
        Message::timerSet(0);
        _recievedMessage = (_recievedMessage << 1) | digitalRead(_busPin);

                if(_debug)
                  Serial.println("Received: " + String(digitalRead(_busPin)) + ", Message: " + String(_recievedMessage,BIN));
            

        if( _recievedMessage >= bit(_controllerNumberWordLen + _pathNumberWordLen + 1))
        {
          if(_debug)
            Serial.println("End recieving! Check for stop bit...");
          if(bitRead(_recievedMessage, 0) == 0)
          {
            Message::messageProcessing();
            _processReceivingMessage = false;

          }
          else 
          {
                  // if(_debug)
                    Serial.println("Bad message! Wrong stop bit. Message is " + String(_recievedMessage) + " " + String(_recievedMessage, BIN));
            _processReceivingMessage = false;
          }
          _recievedMessage = 0;
        }
        Serial.println(String(time1) + " - " + String(millis()));
      }
    }
  }
  
}

void Message::messageProcessing() //21ms
{
  byte controllerNumber = (_recievedMessage & ((bit(_controllerNumberWordLen) - 1) << (_pathNumberWordLen + 1))) >> (_pathNumberWordLen + 1);
  byte pathNumber = (_recievedMessage & ((bit(_pathNumberWordLen) - 1) << 1)) >> 1;
        if(_debug)
          Serial.println("Controller number: " + String(controllerNumber) + "\nPath number: " + String(pathNumber));

  // if(_debug)
    debugCheckMessage(202);

  if(controllerNumber == _thisControllerNumber) // Mask of controller number
  {
            if(_debug)
              Serial.println("Message send to this controller!\nPath light map:" + String(_pathDeviceLightMap[pathNumber],BIN));
    for(int i = 0; i < _deviceAmount; i++)
    {
      digitalWrite(_outputDevicePin[i],bitRead(_pathDeviceLightMap[pathNumber], _deviceAmount - 1 - i));
              if(_debug)
                Serial.println("\t-Pin " + String(_outputDevicePin[i]) + ", state: " + String(bitRead(_pathDeviceLightMap[pathNumber],_deviceAmount - 1 - i)));
    }
  }
  else
  {
            if(_debug)
              Serial.println("Message send to another controller!");
  }
}


void Message::debugRecievedMessageEdit(long fakeReceivedMessage)
{
  _recievedMessage = fakeReceivedMessage;
}

void Message::debugCheckMessage(long expectedRecievedMessage)
{
  if(_recievedMessage == expectedRecievedMessage)
  {
    Serial.println("Message has been successfully checked!");
  }
  else
  {
    Serial.println("Bad message!");
  }
}
