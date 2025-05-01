#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>

class PrintDeviceCallback : public BLEAdvertisedDeviceCallbacks{
  public:
    void onResult(BLEAdvertisedDevice bleAdvertiseDevice);
};
