#include <RCSwitch.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SERIAL_BAUD 9600
#define RECEIVE_CHANNEL 0
#define TRANSMITTER_PIN 10
#define ONE_WIRE_PIN 12

RCSwitch mySwitch = RCSwitch();
String incoming = "";
unsigned long switchCode;

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
unsigned long previousMillis = 0;
unsigned long temperatureInterval = 5000;

void setup() {
  Serial.begin(SERIAL_BAUD);
  
  mySwitch.enableReceive(RECEIVE_CHANNEL);
  mySwitch.enableTransmit(TRANSMITTER_PIN);
  
  sensors.begin();
}

void loop() {
  send();
  receive();
  getTemperature();
}

void receive() {
  if(mySwitch.available()) {
    int value = mySwitch.getReceivedValue();
    if(value != 0) {
      Serial.print("R");
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

void getTemperature() {
  /*if(millis() - previousMillis > temperatureInterval) {
    previousMillis = millis();
    
    sensors.requestTemperatures();
    Serial.print("T");
    Serial.print(sensors.getTempCByIndex(0));
    Serial.print("#");
  }*/
}
