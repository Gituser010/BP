#include "HardwareSerial.h"

#ifndef LORA_PARAM_H
#define LORA_PARAM_H
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
#endif

