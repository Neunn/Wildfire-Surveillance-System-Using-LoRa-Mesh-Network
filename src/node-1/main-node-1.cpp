#include <Arduino.h>
#include <SPI.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <heltec.h>

// ESP32
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 923.2

int nodeIdSelf = 1;
int nodeIdDestination = 4;
int startTimer;
int sendInterval = 2000;

// Singleton instance of the radio driver
RH_RF95 driver(LORA_SS_PIN, LORA_DIO0_PIN);

// Class to manage message delivery and receipt, using the driver declared above
RHMesh *manager;

String message;
String messageResponse = "Alert Received!";

int count = 0;

// ----------------------
// SETUP
// === LoRa
void setup_lora() {  
  manager = new RHMesh(driver, nodeIdSelf);
  
  if (!manager->init()) {
      Serial.println(F("init failed"));
  } else {
      Serial.println("Mesh Node \"" + (String) nodeIdSelf + "\" Up and Running!");
  }
  
  driver.setTxPower(23, false);
  driver.setFrequency(RF95_FREQ);
  driver.setCADTimeout(500);

  Serial.println("RF95 ready");
}

// ----------------------
// ACTIONS
// === LoRa
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

void sendMessage(String _message, int _nodeIdDestination, int _sendMessageCount) {
  for (int i=0; i<_sendMessageCount; i++) {
    Serial.println("Sending to RF95 Mesh Node \"" + (String) _nodeIdDestination + "\"!");
    // Send a message to a rf95_mesh_node
    // A route to the destination will be automatically discovered.

    char messageChar[_message.length() + 1];
    strcpy(messageChar, _message.c_str());
    
    int errorLog = manager->sendtoWait((uint8_t*) messageChar, sizeof(messageChar), nodeIdDestination);
    if (errorLog == RH_ROUTER_ERROR_NONE)
    {
      // It has been reliably delivered to the next node.
      // Now wait for a reply from the ultimate server
      uint8_t len = sizeof(buf);
      uint8_t from;    
      if (manager->recvfromAckTimeout(buf, &len, 3000, &from))
      {
        Serial.print("Got Reply from ");
        Serial.print(from, HEX);
        Serial.println(":");
        Serial.println((char*)buf);}
      else
      {
        Serial.println("No reply, is rf95_mesh_node1, rf95_mesh_node2 and rf95_mesh_node3 running?");
      }
    }
    else
       Serial.println("sendtoWait failed. Are the intermediate mesh nodes running?");
  }
}

void listen_lora() {
  uint8_t len = sizeof(buf);
  uint8_t nodeIdFrom;

  if (manager->recvfromAck(buf, &len, &nodeIdFrom))
  {
    Serial.print("Got Message nodeIdFrom ");
    Serial.print(nodeIdFrom, HEX);
    Serial.println(":");
    Serial.println((char*)buf);

    Serial.println("lastRssi = " + (String) driver.lastRssi());
    
    // Send a reply back to the originator client
    char messageResponseChar[messageResponse.length() + 1];
    strcpy(messageResponseChar, messageResponse.c_str());
    if (manager->sendtoWait((uint8_t*) messageResponseChar, sizeof(messageResponseChar), nodeIdFrom) != RH_ROUTER_ERROR_NONE)
      Serial.println("sendtoWait failed");
  }
}

void setup() {
    Heltec.begin(false,                /*DisplayEnable*/
                 false, /*LoRaEnable*/ /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
                 true,                 /*SerialEnable*/
                 true,                 /*PABOOST*/
                 923.2E6 /*Band*/);
    Serial.begin(9600);
    setup_lora();
    startTimer = millis();
    message = "Hello! from node " + (String) nodeIdSelf + ", to node " + (String) nodeIdDestination;
}

void loop() 
{
  if (millis() - startTimer > sendInterval) {
    startTimer = millis();
    sendMessage(message, nodeIdDestination, 1);
    count++;
    Serial.println((String) count + "/200 sent");
  }
  listen_lora();  
  if (count >= 200) while(1);
}