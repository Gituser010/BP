#include "freertos/idf_additions.h"
#include "LoRaServer.h"
#include "LoRaPackets.h"
#include "PacketHandler.h"

int last_recieved;
LoRaParams defaultParams;
LoRaParams real_params;
LoRaParams * params = &real_params;
RecieverParams recieverParams;
QueueHandle_t messageQueue;

LoRaServer * loRaServer;
String loraData;
volatile bool dataAvaliable  = false;
bool default_params_set;
int timestamp;


#define COMM_ID 49772
#define MESSAGE_LEN 300
#define QUEUE_LEN 30


void LoRaCallbackFunction() {
  dataAvaliable=true;
  timestamp = millis();
}

bool sync_flag=false;
bool send_msg = false;
bool * sendMessage= &send_msg;


void handleTask(void *pvParameters) {
  TaskParams taskParams;

  while(true) {
    if(xQueueReceive(messageQueue, &taskParams, portMAX_DELAY)== pdTRUE)
    {
      Serial.println("handling");
      if(HandleMyPacket(taskParams, COMM_ID,params,sendMessage)) {
        last_recieved = millis();
        sync_flag=false;
      }

    //woks on SF12 BW pekne nejake 
      //else 
      //{
        //sync_flag = false;
        //loRaServer->startListening(LoRaCallbackFunction);
      //}
    }
  }
}






void setup() {
  //BLE
  Serial.begin(115200);
  Serial.print("initialaizing");
 
  //LoRa
  messageQueue = xQueueCreate(QUEUE_LEN, sizeof(TaskParams));
  xTaskCreatePinnedToCore(handleTask, "MsgHandler", 4096, NULL, 4, NULL, 1);
  params->preambleLength=12;
  params->freq=2472;

  defaultParams.bw=1625;
  defaultParams.cr=8;
  defaultParams.sf=12;
  defaultParams.preambleLength=12;
  defaultParams.freq=2472;
  defaultParams.pwr=12;

  (*sendMessage) = false;
  loRaServer = new LoRaServer(5,2,4,15);
  loRaServer->setParams(defaultParams);
  default_params_set=true;
  loRaServer->startListening(LoRaCallbackFunction);


}


uint8_t buffer[255];
int last_sinc;


void loop() {
  //Serial.println("Ahoj");
 
  
  if((*sendMessage)) {
    (*sendMessage)=false;
    Serial.println("sending message");
    loRaServer->sendMessage("OK");
    loRaServer->setParams((*params));
    default_params_set = false;
    params->print();
    loRaServer->startListening(LoRaCallbackFunction);
  }
  if(dataAvaliable == true) {
    dataAvaliable=false;
    //Serial.println("dataAvaliable");
    //reads data
    recieverParams = loRaServer->readToBuffer((char *)buffer,255);
    TaskParams params;
    params.recieverParams=recieverParams;
    params.timestamp=timestamp;
    memcpy(params.data,buffer,255);
    // /uint8_t message[255];
    if(xQueueSend(messageQueue, &params,0) != pdTRUE) {
      
      Serial.println("Queue full! Message dropped");
    }
    loRaServer->startListening(LoRaCallbackFunction);
    Serial.println("len");
    Serial.println(params.recieverParams.len);
  }
  if (millis()-last_recieved >= 10000 && (uxQueueMessagesWaiting(messageQueue) == 0) && (!default_params_set)) {
    Serial.println("trying to connect");
    Serial.println("default params");
    loRaServer->setParams(defaultParams);
    default_params_set = true;
    loRaServer->startListening(LoRaCallbackFunction);
    sync_flag=true;
  }
  
  //delay(1000);
}
