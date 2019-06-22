//Serial Passthrough
//By Ryan Walmsley May 2019
//(C) Pi Supply 2019
//based off the arduino serial example

//This allows you to communicate with the RAK board using a serial terminal on your computer, baudrate 9600

#include "SoftwareSerial.h"
SoftwareSerial DebugSerial(10,11);

void setup() {
  pinMode(12, OUTPUT);
  Serial.begin(9600);
  DebugSerial.begin(9600);

  digitalWrite(12, LOW);

  delay(1000);
    digitalWrite(12, HIGH);
}

void loop() {


  if (Serial.available()) {      // If anything comes in Serial (USB),
    DebugSerial.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (DebugSerial.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(DebugSerial.read());   // read it and send it out Serial (USB)
  }
}
