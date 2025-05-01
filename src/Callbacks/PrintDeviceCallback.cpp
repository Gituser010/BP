#include "PrintDeviceCallback.h"

void PrintDeviceCallback::onResult(BLEAdvertisedDevice bleAdvertiseDevice) {
  Serial.printf("Advertised Device: %d \n",bleAdvertiseDevice.toString());
}
