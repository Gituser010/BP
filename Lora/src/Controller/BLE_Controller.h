#include "BLEServer.h"
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

  public:
    BLEController(String name);
};
#endif
