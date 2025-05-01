#include <BLEAdvertisedDevice.h> 
#include <BLEAddress.h>

class FindDeviceCallback : public BLEAdvertisedDeviceCallbacks{
public:
  FindDeviceCallback(String deviceName,BLEAddress ** pFoundDevAddr);
  void onResult(BLEAdvertisedDevice bleAdvertiseDevice);


private:
  String deviceName;
  BLEAddress ** pFoundDevAddr;
}; 

