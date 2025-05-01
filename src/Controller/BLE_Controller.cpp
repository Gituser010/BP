#include <cstddef>
#include "xtensa/config/specreg.h"
#include "BLEAddress.h"
#include "esp32-hal.h"
#include "BLEScan.h"
#include "HardwareSerial.h"
#include "BLEAdvertisedDevice.h"
#include "BLEDevice.h"
#include "BLE_Controller.h"
#include "../Callbacks/FindDeviceCallback.h"
#include "../Callbacks/PrintDeviceCallback.h"

//if you have dificulties with connecitng increase scan time 
#define SCAN_TIME 10

BLEController::BLEController(String name) {
    BLEDevice::init(name);
  }

BLEScanResults* BLEController::scan() {
  scanSetUp();
  return pBLEScan->start(SCAN_TIME, false);
}

BLEAddress* BLEController::scanFor(String name) {
  scanSetUp();
  //callback function that runs for each found BLE device
  pBLEScan->setAdvertisedDeviceCallbacks(new FindDeviceCallback(name,&pServerAddress));
  pBLEScan->start(SCAN_TIME, false);
  delay(2000);
  return pServerAddress;
}

bool BLEController::connect(BLEAddress *pServerAddress) {
    if(pServerAddress==NULL) {
    return false;
  }
  
  pBLEClient = BLEDevice::createClient();
  pBLEClient->connect(*pServerAddress);

  return true;
}

bool BLEController::disconnect() {
  pBLEClient->disconnect();
  return true;
}


void BLEController::scanSetUp() {
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new PrintDeviceCallback());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}
