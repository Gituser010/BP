#include "BLEServer.h"
#include "BLECharacteristic.h"
#include "BLE_Server_Controller.h"

BLEControllerServer::BLEControllerServer(String name,BLEServerCallbacks * callbackFunction) : BLEController(name) {
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(callbackFunction);
}

bool BLEControllerServer::createService(String service_UUID) {
  this->service_UUID=service_UUID;
  pService = pServer->createService(service_UUID);
  pCharacteristics = pService->createCharacteristic(CHARACTERISTIC_UUID,
  BLECharacteristic::PROPERTY_READ |
  BLECharacteristic::PROPERTY_WRITE);
  pCharacteristics->setValue("Ping");
  pService->start();
  
  return true;
}

bool BLEControllerServer::advertise() {
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(service_UUID);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLEAdvertising started");
  
  return true;
}

