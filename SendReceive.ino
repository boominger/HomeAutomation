#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch(); String incoming = ""; long switchCode; 
void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);
  mySwitch.enableTransmit(10);
}
void loop() {
  send();
  receive();
}
void receive() {
  if(mySwitch.available()) {
    int value = mySwitch.getReceivedValue();
    if(value != 0) {
      Serial.print(mySwitch.getReceivedValue());
      Serial.print("#");
    }
    mySwitch.resetAvailable();
  }
}
void send() {
  if(Serial.available() > 0) {
    incoming = Serial.readStringUntil('#');
    switchCode = incoming.toInt();
    mySwitch.send(switchCode, 24);
  }
}
