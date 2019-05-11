//Baudrate Setter
//By Ryan Walmsley May 2019
//(C) Pi Supply 2019

//This will set the baudrate of the IoT LoRa Node Shield to 9600 Baud

#include "SoftwareSerial.h"
int RESET_PIN = 12;
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial RAKSerial(RXpin,TXpin);    // Declare a virtual serial port
//First reset the RAK board
void setup() {
 //Reset the board

 DebugSerial.begin(9600);
 digitalWrite(RESET_PIN, LOW);   // turn the pin low to Reset
 delay(400);
 digitalWrite(RESET_PIN, HIGH);    // then high to enable
 DebugSerial.println("Reset RAK");
 delay(400);

 //Start communication
 RAKSerial.begin(115200); // Arduino Shield
 RAKSerial.println("at+uart=9600,8,0,0,0");
  delay(400);
 digitalWrite(RESET_PIN, LOW);   // turn the pin low to Reset
 delay(400);
 digitalWrite(RESET_PIN, HIGH);    // then high to enable

 }

void loop() {

  DebugSerial.println("BAUD SHOULD BE SET");
  delay(10000);
}
