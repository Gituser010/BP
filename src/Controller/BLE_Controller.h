#include "BLEAddress.h"
#include "BLEScan.h"
#ifndef BLE_CONNECT_CALLBACK_H
#define BLE_CONNECT_CALLBACK_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>


class BLEController {
  private: 
  BLEScan * pBLEScan;  
  BLEClient * pBLEClient;
  BLEAddress * pServerAddress;

  void scanSetUp();

  public:
    BLEController(String name);
    BLEScanResults* scan();
    BLEAddress* scanFor(String name);
    bool connect(BLEAddress* pServerAddress);
    bool disconnect();
};
#endif
