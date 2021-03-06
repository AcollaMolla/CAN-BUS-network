#include <Arduino_FreeRTOS.h>
#include "mcp2515_can.h"

void sendCAN(void *pvParameters);
void receiveCAN(void *pvParameters);
void temperatureReading(void *pvParameters);
void reply();

const int SPI_CS_PIN = 10;
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
unsigned char response[2] = {0x3e, 0x1};

void setup(){
  SERIAL_PORT_MONITOR.begin(115200);
  while(!Serial){};
  while (CAN_OK != CAN.begin(CAN_125KBPS)) {             // init can bus : baudrate = 500k
    SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
    delay(100);
  }
  
  SERIAL_PORT_MONITOR.println("CAN init ok!");
  xTaskCreate(sendCAN, "sendCAN", 128, NULL, 4, NULL);
  xTaskCreate(receiveCAN, "receiveCAN", 128, NULL, 2, NULL);
  xTaskCreate(temperatureReading, "temperatureReading", 128, NULL, 3, NULL);
  vTaskStartScheduler();
}

void loop(){
  
}

void temperatureReading(void *pvParameters){
  float reading = 0;
  float adc = 0;
  
  while(1){
    reading = analogRead(A0);
    adc = reading/(reading+10000)*3.3*(1023/reading);
    reading = (1023/reading)-1;
    reading = 10000/reading;
    Serial.print("Resistance: ");
    Serial.println(reading);
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void sendCAN(void *pvParameters){
  while(1){
    CAN.sendMsgBuf(0x3fe, 0, 2, response);
    SERIAL_PORT_MONITOR.println("Sending!");
    vTaskDelay(10000/portTICK_PERIOD_MS);
  }
}

void receiveCAN(void *pvParameters){
  unsigned char len = 0;
  unsigned char buf[8];
  while(1){
    if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      unsigned long canId = CAN.getCanId();
      if(buf[0] == 0x40){
        SERIAL_PORT_MONITOR.println("Someone called me!");
        reply();
      }
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void reply(){
  response[0] = random(0x1e, 0xff);
  response[1] = random(0x0, 0xff);
  CAN.sendMsgBuf(0x2fe, 0, 2, response);
  SERIAL_PORT_MONITOR.print("Replied with: ");
  for(int i=0;i<2;i++){
    SERIAL_PORT_MONITOR.print("0x");
    SERIAL_PORT_MONITOR.print(response[i]);
    SERIAL_PORT_MONITOR.print(" ");
  }
  SERIAL_PORT_MONITOR.println();
}
