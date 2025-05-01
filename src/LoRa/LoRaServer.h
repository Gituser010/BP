#include <cstddef>
#include <thread>
#include <RadioLib.h>
#include "LoRaParams.h"
#include "../wrap/LoRapackets.h"

using namespace std;

#ifndef LORA_SERVER_H
#define LORA_SERVER_H

class LoRaServer{
public:
  LoRaServer(int NSS, int DIO1, int NRST, int BUSY);
  LoRaServer(int NSS, int DIO1, int NRST, int BUSY, LoRaParams);
  ~LoRaServer();
  void advertise();
  void stopAdvertising();
  void disconnect();
  void startListening(void (*callbacKFunction)());
  void sendMessage(String message);
  void sendMessage(SyncLoRaPacket12 * message,int len);
  void startSendingMessage(SyncLoRaPacket12 * message,int len);
  void sendMessage(SyncLoRaPacket12 * message);
  SX1280* getRadio();

  String readData();
  static void readRequest();
  LoRaParams getParams();
  void setParams(LoRaParams params);
  static LoRaServer * instance;


private:
  thread advertiseThread;
  atomic<bool> advertissing = false;
  SX1280 radio = new Module(5,2,4,15);
  String name = "SX1280";
  int ack = 1235;
  LoRaParams params;


  void sendPing();
  static void readAdvResponse();
  void readResponse();
  void acceptConnection();
  int checkForErorr(int state);
};

#endif
