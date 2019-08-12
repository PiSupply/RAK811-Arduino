//IoT LoRa Node Shield ABP Example
//By Ryan Walmsley May 2019
//(C) Pi Supply 2019
//Based off RAK Wireless Code

#include "RAK811.h"
#include "SoftwareSerial.h"
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE ABP    //  OTAA or ABP
#if JOIN_MODE == OTAA
String DevEui = "";
String AppEui = "";
String AppKey = "";
#else JOIN_MODE == ABP
String NwkSKey = "FF62772E4535CDBD4DC746168B48DC9D"; // Fill this out
String AppSKey = "D84D890C81CE8F8D29E980E4D9169F9B"; // Fill this out
String DevAddr = "26011E34"; // Fill this out
#endif
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial RAKSerial(RXpin,TXpin);    // Declare a virtual serial port
char* buffer = "72616B776972656C657373";
bool InitLoRaWAN(void);
RAK811 RAKLoRa(RAKSerial,DebugSerial);

int RESET_PIN = 12;


void setup() {
 //Define Reset Pin
 pinMode(RESET_PIN, OUTPUT);
 //Setup Debug Serial on USB Port
 DebugSerial.begin(9600);
 while(DebugSerial.read()>= 0) {}
 while(!DebugSerial);
 //Print debug info
 DebugSerial.println("StartUP");
 DebugSerial.println("Reset");
 //Reset the RAK Module
 digitalWrite(RESET_PIN, LOW);   // turn the pin low to Reset
 digitalWrite(RESET_PIN, HIGH);    // then high to enable
 DebugSerial.println("Success");
 RAKSerial.begin(9600); // Arduino Shield
 delay(100);
 DebugSerial.println(RAKLoRa.rk_getVersion());
 delay(200);
 DebugSerial.println(RAKLoRa.rk_getBand());
 delay(200);

 while (!InitLoRaWAN());
 Serial.println("Pass");

}
bool InitLoRaWAN(void)
{
  RAKLoRa.rk_setWorkingMode(WORK_MODE);
  RAKLoRa.rk_recvData();
  RAKLoRa.rk_recvData();
  if ( RAKLoRa.rk_recvData() == "OK")
  {
    if (RAKLoRa.rk_initABP(DevAddr, NwkSKey, AppSKey))
    {
      Serial.println("You init ABP parameter is OK!");
      if (RAKLoRa.rk_joinLoRaNetwork(JOIN_MODE))
      {
        Serial.println("You join Network success!");
        return true;
      }
    }
  }
  return false;
}

void loop() {
  Serial.println("You join Network success!");
  int packetsflag = 0; // 0: unconfirmed packets, 1: confirmed packets
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
