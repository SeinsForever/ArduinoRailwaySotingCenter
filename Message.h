#pragma once
#include <Arduino.h>

namespace MessageBase
{
  class Message
  {
    public:
      Message(unsigned int workFreaquency, byte thisControllerNumber, byte typeOfThisController, byte controllerNumberWordLen = 2, byte deviceNumberWordLen = 4,
              byte busPin = 2, byte debug = 0);

      void printProperties();

      void printMessage();

      void changeOfMessageProperties(byte newControllerNumberWordLen = NULL, byte newDeviceNumberWordLen = NULL);

      void formingMessage(byte controllerNumber, byte deviceNumber);

      void sendMessageInPort();

      void recieveMessageInPort();

      void messageProcessing();

      void timerSet(byte timerNumb, unsigned long time = millis());

      unsigned long timerRead(byte timerNumb);

      void debugRecievedMessageEdit(long fakeReceivedMessage);

      void debugCheckMessage(long expectedRecievedMessage);

    private:
      /// Formed message
      long _sendingMessage = 0;
      long _recievedMessage = 0;

      /// Message mask
      long _sendingMessageMask = 0;

      /// Process flags
      byte _processSendingMessage = 0;
      byte _processReceivingMessage = 0;

      /// Message properties
      byte _controllerNumberWordLen;
      byte _pathNumberWordLen;

      byte _outputDevicePin[12] = {3,4,5,6,7,8,9,10,11,12,13,14};
      byte _inputDevicePin[0] = {};

      byte _deviceAmount;
      long _pathDeviceLightMap[7] = {0b100000000000,
                                    0b011100000000,
                                    0b011010000000,
                                    0b010001110000,
                                    0b010001101000,
                                    0b010001000110,
                                    0b010001000101};

      long _pathDeviceArrowMap[7] = {0b100000,
                                    0b011000,
                                    0b001000,
                                    0b000000,
                                    0b000100,
                                    0b000010,
                                    0b000011};                 

      /// System properties
      byte _busPin;
      byte _thisControllerNumber;
      unsigned int _workFreaquency;

      unsigned long _timer[10] = {0};

      byte _debug;
  };
}
