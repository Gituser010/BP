#include "PrintDeviceCallback.h"

void PrintDeviceCallback::onResult(BLEAdvertisedDevice bleAdvertiseDevice) {
  Serial.printf("Advertised Device: %s \n",bleAdvertiseDevice.toString());
}
