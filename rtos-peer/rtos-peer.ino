#include <Arduino_FreeRTOS.h>
#include "mcp2515_can.h"

void sendCAN(void *pvParameters);
void receiveCAN(void *pvParameters);

const int SPI_CS_PIN = 10;
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

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
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void receiveCAN(void *pvParameters){
  while(1){
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
