#include <Arduino_FreeRTOS.h>

void sendCAN(void *pvParameters);
void receiveCAN(void *pvParameters);

void setup(){
  xTaskCreate(sendCAN, "sendCAN", 128, NULL, 3, NULL);
  xTaskCreate(receiveCAN, "receiveCAN", 128, NULL, 2, NULL);
  vTaskStartScheduler();
}

void loop(){
  
}

void sendCAN(void *pvParameters){
  while(1){
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void receiveCAN(void *pvParameters){
  while(1){
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
