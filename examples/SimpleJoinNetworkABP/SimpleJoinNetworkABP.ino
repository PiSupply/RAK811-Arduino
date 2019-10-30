//Simple Join Network via ABP Example
//By Ryan Walmsley (C) Pi Supply 2019

//Import the RAK811 Library
#include "RAK811.h"
//Include Serial Library
#include "SoftwareSerial.h"

//Set here if you want to use LoRaWAN or P2P Modes
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
//This sets the code to ABP
#define JOIN_MODE ABP

//Define your ABP Keys here, you can usually find these in the console of the LoRa Provider.
String NwkSKey = "9E8F6BFDFC0DA1A41F67E75B6901830D";
String AppSKey = "34DEA9B1B8A4D53555038583334C4487";
String DevAddr = "26011D9D";

//Specify Data Rate
//For EU & AU This is recommended to be 5 for SF7, For US use 4
String dataRate = "5";


//The Pi Supply shield uses pins 11 & 10 for serial.
#define TXpin 11 
#define RXpin 10
#define RESET_PIN 12
//The DebugSerial allows the board to respond back via the USB Serial
#define DebugSerial Serial
//Setup the serial port for the board
SoftwareSerial RAKSerial(RXpin,TXpin);    // Declare a virtual serial port


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

     Serial.println("Setting data rate");
  RAKLoRa.rk_setConfig("dr", dataRate);
  delay(200);


   while (!InitLoRaWAN());

}

void loop() {
  //The buffer is the data you wish to send in a hex format. Change this variable to be the data you wish to send.
  char* buffer = "52544B";

  //Change between unconfirmed and confirmed packets. Confirmed packets will request the gateway to respond to confirm it has recieved them. However for normal operation unconfirmed packets are recommended.
  int packetsflag = 0; // 0: unconfirmed packets, 1: confirmed packets
  if (RAKLoRa.rk_sendData(packetsflag, 1, buffer))
  {
    for (unsigned long start = millis(); millis() - start < 200000L;)
    {
      DebugSerial.println("Loop!");
      DebugSerial.println(millis());
      String ret = RAKLoRa.rk_recvData();
      DebugSerial.println(ret);
      if (ret.startsWith(STATUS_TX_COMFIRMED) || ret.startsWith(STATUS_TX_UNCOMFIRMED))
      {
        DebugSerial.println("Data Transmitted");
        delay(5000);
        return;
      }
    }
    DebugSerial.println("Error while transmitting");
    
    while(1); 
  }

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
      Serial.println("ABP Keys setup.");
      if (RAKLoRa.rk_joinLoRaNetwork(JOIN_MODE))
      {
        Serial.println("Connecting via ABP");
        return true;
      }
    }
  }
  return false;
}
