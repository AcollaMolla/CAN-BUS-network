#include <Arduino_FreeRTOS.h>
#include <mcp2515.h>

void sendCAN(void *pvParameters);
void receiveCAN(void *pvParameters);

struct can_frame canMsg;
struct can_frame toSend;
struct can_frame ping_request;
struct can_frame ping_ack;

const int SPI_CS_PIN = 10;
MCP2515 mcp2515(SPI_CS_PIN);
int peerRequestCounter = 0;
bool pingReceived = false;
int ping_val1 = 0;
int ping_val2 = 0;
int can_id = 0x0;


void setup(){
  toSend.can_id = can_id;
  toSend.can_dlc = 8;
  toSend.data[0] = 0x44;
  toSend.data[1] = 0xdf;
  toSend.data[2] = 0x12;
  toSend.data[3] = 0x65;
  toSend.data[4] = 0x0;
  toSend.data[5] = 0x5;
  toSend.data[6] = 0x5;
  toSend.data[7] = 0xee;

  ping_request.can_id = can_id;
  ping_request.can_dlc = 2;
  ping_request.data[0] = random(0x0, 0x5);
  ping_request.data[1] = random(0x0, 0x5);
   
  Serial.begin(115200);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");

  xTaskCreate(sendCAN, "sendCAN", 128, NULL, 3, NULL);
  xTaskCreate(receiveCAN, "receiveCAN", 128, NULL, 2, NULL);
  xTaskCreate(sendPing, "Send ping request", 128, NULL, 4, NULL);
  vTaskStartScheduler();
}

void loop(){
  
}

void sendPing(void *pvParameter){
  while(!pingReceived){
      ping_val1 = random(0x5, 0xf);
  ping_val2 = random(0x5, 0xf);
  ping_request.data[0] = ping_val1;
  ping_request.data[1] = ping_val2;
    Serial.print("Sending ping: ");
    Serial.print(ping_request.can_id, HEX);
    Serial.print(" ");
    Serial.print(ping_request.can_dlc, HEX);
    Serial.print(" ");
    for(int i=0;i<2;i++){Serial.print(ping_request.data[i], HEX);Serial.print(" ");}
    Serial.println();  
    mcp2515.sendMessage(&ping_request);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void sendPingAck(int val1, int val2){
  ping_ack.can_id = 0x1;
  ping_ack.can_dlc = 0x2;
  ping_ack.data[0] = val1;
  ping_ack.data[1] = val2;
  mcp2515.sendMessage(&ping_ack);
  Serial.println("Sent ping ack");
}

void sendCAN(void *pvParameters){
  while(1){
    toSend.data[0] = random(0x10, 0xab);
    mcp2515.sendMessage(&toSend);
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void receiveCAN(void *pvParameters){
  unsigned char len = 0;
  unsigned char buf[8];
  while(1){
    if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      if(canMsg.can_id == 0x0 && ((canMsg.data[0] >= 0x5 && canMsg.data[0] <= 0xf) && (canMsg.data[1] >= 0x5 && canMsg.data[1] <= 0xf))){
        //pingReceived = true;
        Serial.println("Ping received!");
        sendPingAck(canMsg.data[0], canMsg.data[1]);
      }
      if(canMsg.can_id = 0x1 && (canMsg.data[0] == ping_request.data[0] && canMsg.data[1] == ping_request.data[1])){
        pingReceived = true;
        can_id = (canMsg.data[0]);
        Serial.println("ACK received from slave");
        Serial.println("Setting CAN ID to: " + can_id);
        toSend.can_id = can_id;
      }
      Serial.print(canMsg.can_id, HEX); // print ID
      Serial.print(" "); 
      Serial.print(canMsg.can_dlc, HEX); // print DLC
      Serial.print(" ");
    
      for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
        Serial.print(canMsg.data[i],HEX);
        Serial.print(" ");
      }
      Serial.println();      
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
