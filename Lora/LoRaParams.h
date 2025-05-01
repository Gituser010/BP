#include "HardwareSerial.h"
#ifndef LORA_PARAMS_H
#define LORA_PARAMS_H

struct LoRaParams{
  float freq;
  float bw;
  int sf;
  int cr;
  int pwr;
  int preambleLength;

  String toString() const {
        return "SF: " + String(sf) +
               ", PWR: " + String(pwr) +
               ", CR: " + String(cr) +
               ", BW: " + String(bw, 3) +
               ", FREQ: " + String(freq);
    }

  void print() const {
    Serial.println(freq);
    Serial.println(bw);
    Serial.println(sf);
    Serial.println(cr);
    Serial.println(pwr);
    Serial.println(preambleLength);
  }
  };

struct RecieverParams {
  int len;
  int RSSI;
  int SNR;
  int state;
  void print() const {
    Serial.println(len);
    Serial.println(RSSI);
    Serial.println(SNR);
  }
};
#pragma pack(push,1)
struct TaskParams { 
  RecieverParams recieverParams;
  char data[255];
  int timestamp;
};
#pragma pack(pop)

#endif