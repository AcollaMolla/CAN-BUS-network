#include <Arduino_FreeRTOS.h>

void Task1(void *pvParameters);
void Task2(void *pvParameters);

void setup(){
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  xTaskCreate(Task1, "task1", 128, NULL, 3, NULL);
  xTaskCreate(Task2, "task2", 128, NULL, 2, NULL);
  vTaskStartScheduler();
}

void loop(){
  
}

void Task1(void *pvParameters){
  pinMode(8, OUTPUT);
  while(1){
    Serial.println("Task1");
    digitalWrite(8, HIGH);
    vTaskDelay(200/portTICK_PERIOD_MS);
    digitalWrite(8, LOW);
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void Task2(void *pvParameters){
  pinMode(7, OUTPUT);
  while(1){
    Serial.println("Task2");
    digitalWrite(7, HIGH);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(7, LOW);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
