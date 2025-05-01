/*
 * file: TestClient.ino
 * description: Wrapper that handles establishing connection and sending LoRa messages
 * author: Martin Pentrak
*/
#include "../LoRa/LoRaServer.h"
#ifndef LORA_WRAP_H
#define LORA_WRAP_H

class LoRaWrapp {
  public:
    int testConnection(LoRaServer * loRaServer,int counter);
};

#endif 
