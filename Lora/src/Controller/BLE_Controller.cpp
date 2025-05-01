#include "BLEServer.h"
#include <cstddef>
#include "xtensa/config/specreg.h"
#include "BLEAddress.h"
#include "esp32-hal.h"
#include "BLEScan.h"
#include "HardwareSerial.h"
#include "BLEAdvertisedDevice.h"
#include "BLEDevice.h"
#include "BLE_Controller.h"
#include "Callbacks/FindDeviceCallback.h"
#include "Callbacks/PrintDeviceCallback.h"

//if you have dificulties with connecitng increase scan time 
#define SCAN_TIME 10
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


BLEController::BLEController(String name) {
    BLEDevice::init(name);
  }

