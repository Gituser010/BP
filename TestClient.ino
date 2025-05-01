/*
 * file: TestClient.ino
 * description: Main Logis of the LoRa Range Test Clinet 
 * Purpose of this code is to send messages of different sizes with differnt lora params
 * to test data rate and max range for each params combination
 * author: Martin Pentrak
*/

#include "./src/wrap/BP_make_conn_wrap.h"
#include "./src/LoRa/LoRaServer.h"
#include "./src/wrap/LoRaConnWrap.h"

#define BUTTON1 25

BLEController* bleController;
ClientWrapp * bleClientWrap;
LoRaWrapp * loRaTestWrap;
LoRaServer * loRaServer;

bool connected = false;

void onButtonPress() {
  static int counter = 0; 
  //connected = bleClientWrap->testConnection(bleController,counter);
  Serial.println("buttonPressed");
  loRaTestWrap->testConnection(loRaServer, counter);
  counter++;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  pinMode(BUTTON1, INPUT_PULLUP);

  bleController = new BLEController("");
  bleClientWrap = new ClientWrapp();

  loRaServer = new LoRaServer(5,2,4,15);
  loRaTestWrap = new LoRaWrapp();
}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(BUTTON1);

  if(buttonState == LOW) {
    onButtonPress();
  }
}