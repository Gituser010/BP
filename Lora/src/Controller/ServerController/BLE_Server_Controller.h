#include "BLEServer.h"
#include "BLECharacteristic.h"
#ifndef BLE_SERVER_H
#define BLE_SERVER_H
#include "../BLE_Controller.h"

#define CHARACTERISTIC_UUID "550e8400-e29b-41d4-a716-446655440000"

class BLEControllerServer : public BLEController {
private:
  BLEServer * pServer; 
  BLEService * pService;
  BLECharacteristic * pCharacteristics;
  String service_UUID;
  BLEAdvertising * pAdvertising;
public: 
  BLEControllerServer(String name,BLEServerCallbacks * callbackFunction);
  bool createService(String service_UUID);
  bool advertise();
  
};
#endif