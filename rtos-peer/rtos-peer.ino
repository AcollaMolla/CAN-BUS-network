#include <Arduino_FreeRTOS.h>
#include "mcp2515_can.h"

void sendCAN(void *pvParameters);
void receiveCAN(void *pvParameters);

const int SPI_CS_PIN = 10;
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
unsigned char stmp[8] = {0x40, 0x1, 0x01, 0x01, 0, 0, 0, 0};

void setup(){
  SERIAL_PORT_MONITOR.begin(115200);
  while(!Serial){};

  while (CAN_OK != CAN.begin(CAN_125KBPS)) {             // init can bus : baudrate = 500k
    SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
    delay(100);
  }
  SERIAL_PORT_MONITOR.println("CAN init ok!");

  xTaskCreate(sendCAN, "sendCAN", 128, NULL, 3, NULL);
  xTaskCreate(receiveCAN, "receiveCAN", 128, NULL, 2, NULL);
  vTaskStartScheduler();
}

void loop(){
  
}

void sendCAN(void *pvParameters){
  while(1){
    CAN.sendMsgBuf(0x000ffffe, 1, 8, stmp);
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void receiveCAN(void *pvParameters){
  unsigned char len = 0;
  unsigned char buf[8];
  while(1){
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      unsigned long canId = CAN.getCanId();
      if(canId == 0x2fe){
        SERIAL_PORT_MONITOR.println("Peer replied!");
        for(int i=0;i<len;i++){
          SERIAL_PORT_MONITOR.print(buf[i], HEX);SERIAL_PORT_MONITOR.print("\t");
        }
      }
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
