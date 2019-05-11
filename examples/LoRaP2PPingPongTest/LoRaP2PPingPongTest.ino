

#include "RAK811.h"
#include "SoftwareSerial.h"
#define WORK_MODE LoRaP2P  //  LoRaWAN or LoRaP2P
#define CODE  PING  // PING or PONG
#define TXpin 11
#define RXpin 10
#define ATSerial Serial
SoftwareSerial DebugSerial(RXpin,TXpin);
char* buffer = "72616B776972656C657373";
String FREQ = "868000000";  // frequency     range:(860000000 - 1020000000)

RAK811 RAKLoRa(ATSerial, DebugSerial);

void setup() {
  // put your setup code here, to run once:
  DebugSerial.begin(9600);
  while(DebugSerial.read()>= 0) {}
  while(!DebugSerial);
  DebugSerial.println("StartUP");

  ATSerial.begin(9600); // Note: Please manually set the baud rate of the WisNode device to 9600.
  delay(100);
  DebugSerial.println(RAKLoRa.rk_getVersion());
  delay(200);
  DebugSerial.println(RAKLoRa.rk_getBand());
  delay(200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (RAKLoRa.rk_setWorkingMode(WORK_MODE))
  {
    DebugSerial.println("You set Working mode is OK!");
    if (RAKLoRa.rk_initP2P(FREQ,7,0,1,8,14))
    {
      DebugSerial.println("You init P2P parameter is OK!");
      while (1)
      {
        #if  CODE == PING
         if (RAKLoRa.rk_sendP2PData(1,"10",buffer))
         {
          String ver = RAKLoRa.rk_recvData();
          //DebugSerial.println(ver);
          if (ver == STATUS_P2PTX_COMPLETE)
          {
            DebugSerial.println("send success");
            if (RAKLoRa.rk_recvP2PData(1))
            {
              DebugSerial.println("open recv");
              DebugSerial.println(RAKLoRa.rk_recvData());
            }
          }
          else
          {
            DebugSerial.println(ver);
            delay(2000);
          }
         }
        #else if CODE == PONG
        if (RAKLoRa.rk_recvP2PData(1))
         {
          DebugSerial.println("open recv");
          DebugSerial.println(RAKLoRa.rk_recvData());
          if (RAKLoRa.rk_sendP2PData(1,"10",buffer))
          {
            String ver = RAKLoRa.rk_recvData();
            if (ver == STATUS_P2PTX_COMPLETE)
            {
              DebugSerial.println("send success");
            }
            else
            {
              DebugSerial.println(ver);
              delay(2000);
            }
          }
         }
        #endif
      }
    }
  }

}
