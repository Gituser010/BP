#include <stdint.h>
#include <iterator>
#include "WString.h"
#include "LoRaServer.h"
#include "../wrap/LoRapackets.h"


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
    Serial.println("OK");
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
  String data = "";
  radio.receive(data);
  Serial.println("recieved");
  return data;
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
    Serial.println("connection request accepted ...");
    Serial.println("sending ping ...");
    stopAdvertising();
    sendPing();
    startListening(NULL);
  }
}

void LoRaServer::advertise() {
  Serial.println("Advertising...");
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
  //Serial.println("Sending message...");
  int state = radio.transmit(message);
  radio.standby();
  checkForErorr(state);
}

void LoRaServer::sendMessage(SyncLoRaPacket12 * data,int len = sizeof(SyncLoRaPacket12)) {
  Serial.println("sendMessage");
  int state=0;
  /*if(len>20)
  {
    if(len<150)
    {
      Serial.println("sendMediumMessage");
      Medium_packet * medium_packet = (Medium_packet *)data;
      Flat_Medium_packet message;
      message.data = (*medium_packet->data); 
      state = radio.transmit((uint8_t *)data, len);
      //return;
    }
    else
    {
          Serial.println("sendBigMessage");
        Big_packet * big_packet= (Big_packet *) data;
        Flat_big_packet big_message;
        big_message.data = (*big_packet->data);
        state = radio.transmit((uint8_t *)data, len);
        //return;
    }
  }
  else
  {*/
    state = radio.transmit((uint8_t *)data, len);
  //}
  

  if(state != RADIOLIB_ERR_NONE) {
    //Serial.println("Packt sent");
    Serial.print("Transimission failed, pkt_id:");
    Serial.print(data->packet_id);
    Serial.println(state);
  }
  radio.standby();
}

void LoRaServer::startSendingMessage(SyncLoRaPacket12 * data,int len = sizeof(SyncLoRaPacket12)) {
  int state = radio.startTransmit((uint8_t *)data, len);
  if(state != RADIOLIB_ERR_NONE) {
    //Serial.println("Packt sent");
    Serial.print("Transimission failed, pkt_id:");
    Serial.print(data->packet_id);
    Serial.println(state);
  }
}

void LoRaServer::sendMessage(SyncLoRaPacket12 * data) {

  //Serial.println("message send");
  int len = sizeof(SyncLoRaPacket12);
  int state = radio.transmit((uint8_t *)data, len);

  if(state != RADIOLIB_ERR_NONE) {
    Serial.print("Transimission failed, code:");
    Serial.println(state);
  }
  radio.standby();
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

/*
String LoRaServer::listen() {
  String data;
  radio.recieve(data,5000);
  return data;
}*/

void LoRaServer::disconnect() {
  String str;
  radio.readData(str);
}

SX1280 * LoRaServer::getRadio() {
  return &(this->radio);
}


void LoRaServer::stopAdvertising() { 
  advertissing = false;
  if (advertiseThread.joinable()) {
    advertiseThread.join();
  }
}



