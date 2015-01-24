#include <EEPROM.h>
#include <RCSwitch.h>
#include <OneWire.h>
#include <DallasTemperature.h>


//Constants
#define SERIAL_BAUD 9600
#define SERIAL_SEPARATOR '\n'

#define RECEIVE_CHANNEL 0
#define TRANSMITTER_PIN 10
#define TRANSMIT_BITLENGTH 24

#define ONEWIRE_PIN 12


//433MHz Transmitter & Receiver
RCSwitch radio = RCSwitch();
String serialInput = "";
unsigned long radioCode;

//Temperature Measurement
OneWire oneWire(ONEWIRE_PIN);
DallasTemperature temperatureSensors(&oneWire);
unsigned long temperaturePrevMillis = 0;
unsigned long measureTemperatureInterval = 10000;


void setup() {
  Serial.begin(SERIAL_BAUD);
  
  radio.enableReceive(RECEIVE_CHANNEL);
  radio.enableTransmit(TRANSMITTER_PIN);
  
  temperatureSensors.begin();
}

void loop() {
  send();
  receive();
  measureTemperature();
}

void receive() {
  if(radio.available()) {
    int value = radio.getReceivedValue();
    if(value != 0) {
      Serial.print("R");
      Serial.print(radio.getReceivedValue());
      Serial.print(SERIAL_SEPARATOR);
    }
    radio.resetAvailable();
  }
}

void send() {
  if(Serial.available() > 0) {
    serialInput = Serial.readStringUntil(SERIAL_SEPARATOR);
    radioCode = serialInput.toInt();
    radio.send(radioCode, TRANSMIT_BITLENGTH);
  }
}

void measureTemperature() {
  if(millis() - temperaturePrevMillis > measureTemperatureInterval) {
    temperaturePrevMillis = millis();
    
    temperatureSensors.requestTemperatures();
    Serial.print("T");
    Serial.print(temperatureSensors.getTempCByIndex(0));
    Serial.print(SERIAL_SEPARATOR);
  }
}
