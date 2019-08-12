

#include "RAK811.h"
#include "SoftwareSerial.h"
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE OTAA    //  OTAA or ABP
#if JOIN_MODE == OTAA
String DevEui = "00F9D51F899A20BD"; // Fill this out
String AppEui = "70B3D57ED0013D25"; // Fill this out
String AppKey = "AB191E16516ED3C5D8534240C29BF557"; // Fill This out
#else JOIN_MODE == ABP
String NwkSKey = "";
String AppSKey = "";
String DevAddr = "";
#endif
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial RAKSerial(RXpin,TXpin);    // Declare a virtual serial port
char* buffer = "72616B776972656C657373";
int RESET_PIN = 12;
bool InitLoRaWAN(void);
RAK811 RAKLoRa(RAKSerial,DebugSerial);


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

}
bool InitLoRaWAN(void)
{
  RAKLoRa.rk_setWorkingMode(WORK_MODE);
  RAKLoRa.rk_recvData();
  RAKLoRa.rk_recvData();
  if ( RAKLoRa.rk_recvData() == "OK")
  {

    if (RAKLoRa.rk_initOTAA(DevEui, AppEui, AppKey))
    {
      DebugSerial.println("You init OTAA parameter is OK!");
      while (RAKLoRa.rk_joinLoRaNetwork(JOIN_MODE))
      {
        bool flag = false;
        for (unsigned long start = millis(); millis() - start < 90000L;)
        {
          String ret = RAKLoRa.rk_recvData();
          if (ret.startsWith(STATUS_JOINED_SUCCESS))
          {
            DebugSerial.println("You join Network success!");
            return true;
          }
          else if (ret.startsWith(STATUS_RX2_TIMEOUT) || ret.startsWith(STATUS_JOINED_FAILED))
          {
            DebugSerial.println("You join Network Fail!");
            flag = true;
            DebugSerial.println("The device will try to join again after 5s");
            delay(1000);
          }
        }
        if (flag == false)
        {
          DebugSerial.println("Pleases Reset the module!");
          delay(1000);
          return false;
        }
      }
    }
  }
     return false;
}

void loop() {
  
   DebugSerial.println("start!");
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
    while (1);
  }
}

