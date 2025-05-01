#include "esp32-hal.h"
#include "HardwareSerial.h"
#include "BP_make_conn_wrap.h"
#define SERVER_NAME "MyEsp32"
int ClientWrapp::testConnection(BLEController * bleController,int counter) {
  int connected = 0;

  Serial.print("Tryiing to connect to ");
  Serial.println(SERVER_NAME);
  BLEAddress * pServerAddres = bleController->scanFor(SERVER_NAME); 
  if(pServerAddres!= NULL) {
    Serial.println("Scan done!");
    for (int i=0; i<10; i++) {
      Serial.println("cycle1");
      if(bleController->connect(pServerAddres)) {
        Serial.print(counter);
        Serial.println(": connected");
        connected=1;
        break;
      }
    }
  }
  delay(2000);
  Serial.println(bleController->disconnect());
  Serial.println("disconnected");
  return connected;
}

