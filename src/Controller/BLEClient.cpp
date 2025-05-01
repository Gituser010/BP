#include "BLEClient.h"
/*
BLEClient::BLEClient(String name) {
  BLEController(name);
}

void BLEController::scanSetUp() {
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new PrintDeviceCallback());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
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
*/