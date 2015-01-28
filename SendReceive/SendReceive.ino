//#include <EEPROM.h>
#include <RCSwitch.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>

//Constants
#define SERIAL_BAUD 9600
#define SERIAL_SEPARATOR '\n'

#define RECEIVE_CHANNEL 0
#define TRANSMITTER_PIN 8
#define TRANSMIT_BITLENGTH 24

#define ONEWIRE_PIN 9

#define BUFSIZE 255


//433MHz Transmitter & Receiver
RCSwitch radio = RCSwitch();
String serialInput = "";
//unsigned long radioCode;

//Temperature Measurement
OneWire oneWire(ONEWIRE_PIN);
DallasTemperature temperatureSensors(&oneWire);
unsigned long temperaturePrevMillis = 0;
unsigned long measureTemperatureInterval = 10000;

//Webserver
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x83, 0x4C };
IPAddress ip(192, 168, 1, 14);
EthernetServer server(80);

void setup() {
  Serial.begin(SERIAL_BAUD);
  
  radio.enableReceive(RECEIVE_CHANNEL);
  radio.enableTransmit(TRANSMITTER_PIN);
  
  temperatureSensors.begin();
  
  Ethernet.begin(mac, ip);
  Serial.print("Webserver running at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  receive();
  serveData();
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

void send(unsigned long radioCode) {
  if(radioCode != 0) {
    Serial.print(radioCode);
    radio.send(radioCode, TRANSMIT_BITLENGTH);
  }
}

float measureTemperature() {
  temperatureSensors.requestTemperatures();
  return temperatureSensors.getTempCByIndex(0);
}

void serveData() {
  char clientline[BUFSIZE];
  int index = 0;
 
  EthernetClient client = server.available();
 
  if(client) {
    index = 0;
    while(client.connected()) {
      if(client.available()) {
        char c = client.read();
        if(c != '\n' && c != '\r') {
          clientline[index] = c;
          index++;
          if(index >= BUFSIZE) {
            index = BUFSIZE -1;
          }
          continue;
        }
        
        String urlString = String(clientline);
        String op = urlString.substring(0,urlString.indexOf(' '));
        urlString = urlString.substring(urlString.indexOf('/'), urlString.indexOf(' ', urlString.indexOf('/')));
        urlString.toCharArray(clientline, BUFSIZE);
        
        String action = strtok(clientline,"/");
        String value = strtok(NULL,"/");
 
        String jsonOut = String();
        if(action != NULL && value != NULL) {
          if(action == "socket") {
            send(value.toInt());
            printOutput(200, client, jsonOutput(action, value, true));
          } else if(action == "temp") {
            String outputValue = String(measureTemperature());
            printOutput(200, client, jsonOutput(action, outputValue, true));
          }
        } else {
          printOutput(200, client, jsonOutput(action, value, false));
        }
        break;
      }
    }
    delay(1);
    client.stop();
  }
}

String jsonOutput(String action, String value, boolean status) {
  String output = String();
  output += "{\"";
  output += "success\":\"";
  output += status;
  output += "\",\r\n \"";
  output += "action\":\"";
  output += action;
  output += "\",\r\n \"";
  output += "value\":\"";
  output += value;
  output += "\"}";

  return output;
}
 
void printOutput(int status, EthernetClient client, String message) {
  switch (status) {
    default:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Access-Control-Allow-Origin: *");
      client.println();
      client.println(message);
      break;
  }
}
