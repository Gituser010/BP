/*
 * file: LoRaTest.cpp
 * description: Test priepustnosti cez LoRa s dynamickými parametrami (SF, CR, BW, Power).
 * author: Tvoje Meno
 * date: 4/2025
 */
#include "freertos/idf_additions.h"
#include <cstdint>
#include "HardwareSerial.h"
#include <cstddef>
#include "LoRaConnWrap.h"
#include "LoRapackets.h"
#include <string>

#define COMM_ID 49772

#define MIN_SF 7
#define MAX_SF 12

#define MIN_OUT_PWR -18
#define MAX_OUT_PWR 12

#define MIN_CR 5
#define MAX_CR 8

#define MIN_FREQ 2400
#define MAX_FREQ 2500

float bw[] = {203.125, 406.25, 812.5, 1625.0};
int pwr[] = {0, 6, 12};
volatile int message_sending = 0;

LoRaServer * localLora=NULL;

QueueHandle_t radioTxQueue;
SemaphoreHandle_t radioSemaphore;

volatile bool txDone = true;
TaskHandle_t txTaskHandle = NULL;


void setTxFlag(void) {
  message_sending-=1;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(txTaskHandle, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

bool enqueueLoRaPacket(const void* data, int len) {

  DynamicPacket pkt;
  pkt.data = (uint8_t*)malloc(len);
  if (!pkt.data) {
    Serial.println("malloc failed!");
    return false;
  }
  if(len>100 && len<200) {
    //Serial.println("medium PAcket");
      Flat_Medium_packet mp;
      mp.data=*(((Medium_packet*)data)->data);
      memcpy(pkt.data,&mp,len);
  }
  else if(len<100) {
    //Serial.println("copping");
    memcpy(pkt.data,data,len);
  }
  else {
    //Serial.println("BigPacket");
    Flat_big_packet bp;
    bp.data = *(((Big_packet*)data)->data);
    memcpy(pkt.data,&bp,len);
  }

  //Serial.println("len assign");
  pkt.length = len;
  //Serial.println("enquePacket");

  if (xQueueSend(radioTxQueue, &pkt, 0) != pdTRUE) {
    Serial.println("send failed");
    return false;
  }
  else {
    message_sending +=1;
  }

  return true;
}

void radioTransmitTask(void* param) {
  bool first = true;
  localLora->getRadio()->setDio1Action(setTxFlag);
  while (true) {
    DynamicPacket pkt;
    if (xQueueReceive(radioTxQueue, &pkt, portMAX_DELAY)) {
        if(first) {
          first=false;
          xSemaphoreTake(radioSemaphore,portMAX_DELAY);
          localLora->startSendingMessage((SyncLoRaPacket12 *)(pkt.data), pkt.length);  // Real send here
          xSemaphoreGive(radioSemaphore);
        }
        else{
        xSemaphoreTake(radioSemaphore,portMAX_DELAY);
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        localLora->startSendingMessage((SyncLoRaPacket12 *)(pkt.data), pkt.length);  // Real send here
        xSemaphoreGive(radioSemaphore);
      //if (localLora && pkt && pkt->data) {
        }
    
      //}      
    }
    free(pkt.data);
  }
}

bool answer(LoRaServer * loRaServer) {
  String ack="1";
  while(ack.length() != 0) {
    ack = "";
    ack = loRaServer->readData();

  //Serial.println("ack");
    Serial.println(ack);
    if(ack.startsWith("OK"))
    {
    //Serial.println("ACK recieved");
      loRaServer->getRadio()->standby();
      return true;
    }
  }
  loRaServer->getRadio()->standby();
  return false;
}


int LoRaWrapp::testConnection(LoRaServer *loRaServer, int counter) {
  localLora=loRaServer;
  radioTxQueue = xQueueCreate(50, sizeof(DynamicPacket));
  radioSemaphore = xSemaphoreCreateBinary();
  xTaskCreatePinnedToCore(radioTransmitTask,"Radio TX Task",4096,NULL,4,&txTaskHandle,1);

  xSemaphoreGive(radioSemaphore);

  Serial.println("testing conn");
  LoRaParams params;
  LoRaParams defaultParams;
  defaultParams.sf=12;
  defaultParams.cr=8;
  defaultParams.bw=bw[3];
  defaultParams.preambleLength=12;
  defaultParams.freq=2472;
  defaultParams.pwr=pwr[2];
  params=defaultParams;
  //int num = 0;  
  bool params_set=true;
  params.freq = 2472;
  params.preambleLength=12;
  SyncLoRaPacket12 sync_packet;
  SyncLoRaPacket12 * sync_message = &sync_packet;

  SyncLoRaPacket12 packet;
  packet.packet_id=0;
  SyncLoRaPacket12 * message = &packet;

  Medium_packet mPacket;
  mPacket.data = &packet;
  Medium_packet * medium_message = &mPacket;

  Big_packet bPacket;
  bPacket.data = &packet;
  Big_packet * big_message=&bPacket;
  bool first_time = true;

  //for(int pwr_i = 0; pwr_i <= 2;pwr_i++) 
   //{
    params.pwr=9;
    for(int sf = MAX_SF; sf >= MIN_SF;sf--)
    {
      params.sf=sf;
      for(int bw_i = 3; bw_i >= 0; bw_i--) 
      { 
        params.bw = bw[bw_i];
        for(int cr = MIN_CR; cr <= MAX_CR; cr++)
        {
          params.cr=cr;    
            //for(int freq = MIN_FREQ; freq <= MAX_FREQ; freq=freq+10)
            //{
              if(!first_time) {
              int retry = 0;
              while (!params_set) {
                while(message_sending>0);
                if(retry<2)
                {
                  Serial.println("if");
                  //params.print();
                  sync_message->setParams(counter,COMM_ID,packet.packet_id,SYNC_PACKET_T,(uint32_t)millis(),params.sf,params.cr,bw_i,params.pwr);
                  //enqueueLoRaPacket(sync_message, sizeof(SyncLoRaPacket12));
                  //localLora->getRadio()->isBusy()
                  //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
                  //enqueueLoRaPacket(sync_message, sizeof(Lora_syn))
                  xSemaphoreTake(radioSemaphore,portMAX_DELAY);
                  loRaServer->sendMessage(sync_message);
                      //Serial.println(message);
                    if(answer(loRaServer)) {
                      //indicates that params was set on Server side
                      params_set=true;
                      //Serial.println("serial answered");
                      break;
                    }

                    else {
                      retry++;
                    }
                    Serial.println("after answer");
                  xSemaphoreGive(radioSemaphore);

                }
        
                else
                {
                  Serial.println("else");
                  //defaultParams.print();
                  loRaServer->setParams(defaultParams);
                  sync_message->setParams(counter,COMM_ID,packet.packet_id,SYNC_PACKET_T,(uint32_t)millis(),params.sf,params.cr,bw_i,6);
                  //enqueueLoRaPacket(sync_message, sizeof(SyncLoRaPacket12));
                  loRaServer->sendMessage(sync_message);
                      //Serial.println(message);
                    if(answer(loRaServer)) {
                      //indicates that params was set on Server side
                      params_set=true;
                      //Serial.println("serial answered");
                      break;
                    }
                }
              }
              }
              message_sending=0;
              first_time = false;
              xSemaphoreGive(radioSemaphore);

       
              loRaServer->setParams(params);
              //params.print();
              //num++;
              
              message->setParams(counter,COMM_ID,packet.packet_id,SMALL_PACKET_T,(uint32_t)millis(),params.sf,params.cr,bw_i,params.pwr);
              //String message = "num: " + String(num) + " id:" + String(counter) + " params: " + params.toString() + " next_params: " + next_params.toString();
              //send 10 short messages with time sync
              for(int i = 0;i <10; i++) {
                ///Serial.println("for");
                //loRaServer->sendMessage(message);
                enqueueLoRaPacket(message, sizeof(SyncLoRaPacket12));
                packet.packet_id++;
              }

              //send 10 medium long messages
              medium_message->data = &packet;
              medium_message->data->setParams(counter,COMM_ID,packet.packet_id,MEDIUM_PACKET_T,(uint32_t)millis(),params.sf,params.cr,bw_i,params.pwr);
              
              for(int i = 0;i <10; i++) {
                //Serial.println("for");
                //medium_message->data.packet_id = num;
                //loRaServer->sendMessage((SyncLoRaPacket12 *)medium_message,sizeof(Medium_packet));
                enqueueLoRaPacket(medium_message, sizeof(Flat_Medium_packet));
                //Serial.println(num);
                packet.packet_id++;
                //num++;
              }
              //Serial.println("hii");
              big_message->data = &packet;
              big_message->data->setParams(counter,COMM_ID,packet.packet_id,BIG_PACKET_T,(uint32_t)millis(),params.sf,params.cr,bw_i,params.pwr);
              //send 10 long messages 
              for(int i = 0;i <10; i++) {
                //big_message->data.packet_id = num;
                //loRaServer->sendMessage((SyncLoRaPacket12 *)big_message,sizeof(Big_packet));
                enqueueLoRaPacket(big_message, sizeof(Flat_big_packet));
                //num++;
                packet.packet_id++;
              }

              //try 3 times to connect and sync new params 
          //num=packet.packet_id;
              params_set = false;
            //}
           while (uxQueueMessagesWaiting(radioTxQueue) != 0) {
            delay(100);
           } 
        
          }
        }
     }
  // }
  return 0;
}



