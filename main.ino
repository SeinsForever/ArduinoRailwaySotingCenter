#include "Message.h"
using namespace MessageBase;

void setup() 
{
  Serial.begin(9600);
}


void loop() 
{
  
  /// message(
  ///        1 - working frequency,
  ///        2 - the number of this controller on the net,
  ///        3 - type of this controller (0 - ligh
  ///                                 1 - arrows)
  ///        ---optional---
  ///        4 - controller number word length in message (default = 2),
  ///        5 - device number word length in message (default = 4),
  ///        6 - bus pin number (default = 2),
  ///        7 - debug (0 - off (default), 1 - on)
  ///        )
 
  // static Message message(100,1,0,2,4,2,0);

  // if(millis() - message.timerRead(2) >= 10000)
  // {
  //   // Serial.println("Waiting new message...");

  //   message.timerSet(2);
  //   message.printProperties();
  //   message.formingMessage(2, 5);
  //   message.printMessage();
  // }

  // message.sendMessageInPort();


  static Message message(100,2,0,2,4,2,0);

  if(millis() - message.timerRead(3) >= 15000)
  {
    message.timerSet(3);
    // message.debugRecievedMessageEdit(230);
    // message.messageProcessing();
  }

  message.recieveMessageInPort();


}
