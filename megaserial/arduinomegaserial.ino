#include "SoftwareSerial.h"
#include "SerialManager.h"

SerialManager serial;

void setup() {
  serial.start(1);
  Serial.begin(115200);
}

void loop() {
   if (serial.onReceive()){
    Serial.println("---Start---");
    Serial.println(serial.getCmd().toInt());
    Serial.println(serial.getParam());
    //serial.println(serial.getValue());
    Serial.println("---End---");
  }
}
