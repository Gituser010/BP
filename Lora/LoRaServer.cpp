#include <cstdint>
#include "HardwareSerial.h"
#include "esp32-hal.h"
#include "WString.h"
#include "LoRaServer.h"


LoRaServer * LoRaServer::instance = nullptr;

LoRaServer::LoRaServer(int NSS, int DIO1, int NRST,int BUSY) {
 // sx1280 = new Module(NSS,DIO1,NRST,BUSY); 
  int state = radio.begin();
  checkForErorr(state);
  instance = this;
}

LoRaServer::LoRaServer(int NSS, int DIO1, int NRST, int BUSY, LoRaParams params) {
  //sx1280 = new Module(int NSS,int DIO1,int NRST,int BUSY);
  int state = radio.begin(params.freq,params.bw,params.sf,params.cr,params.pwr,params.preambleLength);
  checkForErorr(state);
  instance = this;
}
int LoRaServer::checkForErorr(int state) {
  if(state==RADIOLIB_ERR_NONE) {
    return state;
  }
  else {
    Serial.print("Error ");
    Serial.println(state);
    return state;
  }
}

void LoRaServer::readAdvResponse() {
  
  instance->readResponse();
  /*int state = radio.readData(data);
  checkForErorr(state);
  int ack_plus_1 = ack + 1;
  String expectedResponse = this->name + ";" + ack_plus_1;
  if(data.compareTo(expectedResponse)) {
    Serial.println("connection request accepted ...");
    Serial.println("sending ping ...");
    stopAdvertising();
    sendPing();
    startListening();
  }*/
}

String LoRaServer::readData() {
  String data;
  radio.readData(data);
  radio.standby();
  return data;
}

RecieverParams LoRaServer::readToBuffer(char * buffer,int size) {
  RecieverParams params;
  params.len = radio.getPacketLength();
  params.RSSI = radio.getRSSI();
  params.SNR = radio.getSNR();
  params.state = radio.readData((uint8_t *)buffer,size);
  radio.standby();
  return params;
  
}

LoRaParams LoRaServer::getParams() {
  return this->params;
}

void LoRaServer::setParams(LoRaParams params) {
  //putting radio to standby mode to set params
  radio.standby();
  this->params = params;

  int status;
  Serial.println(this->params.cr);
  //seting prameter for radio
  if((status = radio.setCodingRate(params.cr,false))) {
    Serial.println("CR: Error");
    Serial.println(status);
  }
  if(radio.setBandwidth(params.bw)) {
    Serial.println("BW: Error");
  }
  if(radio.setSpreadingFactor(params.sf)) {
    Serial.println("SF: Error");
  }
  if(radio.setFrequency(params.freq)) {
    Serial.println("Freq: Error");
  }

  if(radio.setPreambleLength(params.preambleLength)) {
    Serial.println("PreambleLen: Error");
  }

  if(radio.setOutputPower(params.pwr)) {
    Serial.println("PWR: Error");
  }
};

void LoRaServer::readResponse()
{
    String data;
  int state = radio.readData(data);
  checkForErorr(state);
  int ack_plus_1 = ack + 1;
  String expectedResponse = this->name + ";" + ack_plus_1;
  if(data.compareTo(expectedResponse)) {
    stopAdvertising();
    sendPing();
  }
}


void LoRaServer::advertise() {
  advertissing = true;
  advertiseThread = thread([this]{
    String advertisse_string = this->name + ";" + this->ack;
    while(advertissing.load()){
      int state = radio.transmit(advertisse_string);
      checkForErorr(state);
    }
  });
  radio.setPacketReceivedAction(LoRaServer::readAdvResponse);
  radio.startReceive();
}

void LoRaServer::sendMessage(String message) {
  int state = radio.transmit(message);
  radio.standby();
  checkForErorr(state);
}


void LoRaServer::sendPing() {
  ack=ack+2;
  sendMessage("PING");
} 

void LoRaServer::readRequest() {
  instance->readResponse();
}

void LoRaServer::startListening(void (*callbacKFunction)()) {
  radio.setPacketReceivedAction(callbacKFunction);
  radio.startReceive();
}


void LoRaServer::disconnect() {
  String str;
  radio.readData(str);
}


void LoRaServer::stopAdvertising() { 
  advertissing = false;
  if (advertiseThread.joinable()) {
    advertiseThread.join();
  }
}




