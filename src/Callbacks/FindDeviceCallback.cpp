#include "HardwareSerial.h"
#include "FindDeviceCallback.h"

FindDeviceCallback::FindDeviceCallback(String deviceName,BLEAddress ** pFoundDevAddr) {
  this->deviceName=deviceName;
  this->pFoundDevAddr=pFoundDevAddr;
  *(this->pFoundDevAddr)=nullptr;
}

void FindDeviceCallback::onResult(BLEAdvertisedDevice bleAdvertiseDevice) {
  Serial.println(bleAdvertiseDevice.toString());
  if(bleAdvertiseDevice.getName()==deviceName) {
    Serial.println("matched");
    bleAdvertiseDevice.getScan()->stop();
    (*pFoundDevAddr) = new BLEAddress(bleAdvertiseDevice.getAddress());
    Serial.println("Device Found..");
    Serial.println("Connecting..");
  }
}
