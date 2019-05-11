

#include "RAK811.h"
#include "SoftwareSerial.h"
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE ABP    //  OTAA or ABP
#if JOIN_MODE == OTAA
String DevEui = "60C5A8FFFE000001";
String AppEui = "70B3D57EF00047C0";
String AppKey = "5D833B4696D5E01E2F8DC880E30BA5FE";
#else JOIN_MODE == ABP
String NwkSKey = "DAC93A91C7C258F8403BC68C50381127";
String AppSKey = "439F5ACF10FCFBED7D54485D2CF1CE15";
String DevAddr = "260113FF";
#endif
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define ATSerial Serial
SoftwareSerial DebugSerial(RXpin,TXpin);    // Declare a virtual serial port
char* buffer = "72616B776972656C657373";

bool InitLoRaWAN(void);
RAK811 RAKLoRa(ATSerial,DebugSerial);


void setup() {
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

 while (!InitLoRaWAN());

}

bool InitLoRaWAN(void)
{
  if (RAKLoRa.rk_setWorkingMode(WORK_MODE))
  {
    if (RAKLoRa.rk_initABP(DevAddr, NwkSKey, AppSKey))
    {
      DebugSerial.println("You init ABP parameter is OK!");
      if (RAKLoRa.rk_joinLoRaNetwork(JOIN_MODE))
      {
        DebugSerial.println("You join Network success!");
        return true;
      }
    }
  }
  return false;
}

void loop() {
  int packetsflag = 1; // 0: unconfirmed packets, 1: confirmed packets
  if (RAKLoRa.rk_sendData(packetsflag, 1, buffer))
  {
    for (unsigned long start = millis(); millis() - start < 90000L;)
    {
      String ret = RAKLoRa.rk_recvData();
      if (ret.startsWith(STATUS_TX_COMFIRMED) || ret.startsWith(STATUS_TX_UNCOMFIRMED))
      {
        DebugSerial.println("Send data ok!");
        delay(5000);
        return;
      }
    }
    DebugSerial.println("Send data error!");
    while(1);
  }
}
