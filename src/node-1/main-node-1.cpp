/// ส่งได้ดีมาก

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <heltec.h>
#include <RH_RF95.h>
#include <RHMesh.h>
#include <HT_SSD1306Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 923.2

int nodeIdSelf = 1;
int nodeIdDestination = 4;

// Singleton instance of the radio driver
RH_RF95 driver(18, 26);

// Class to manage message delivery and receipt, using the driver declared above
RHMesh *manager;

int sess_start, sendTimeoutStart, delivCount;

String message = "Hello! from node " + (String)nodeIdSelf + ", to node " + (String)nodeIdDestination;
String messageResponse = "Alert Received!";

void setup_lora()
{

  manager = new RHMesh(driver, nodeIdSelf);

  if (!manager->init())
  {
    Serial.println(F("init failed"));
  }
  else
  {
    Serial.println("Mesh Node \"" + (String)nodeIdSelf + "\" Up and Running!");
  }

  driver.setTxPower(23, false);
  driver.setCADTimeout(500);
  driver.setFrequency(RF95_FREQ);
  Serial.println("RF95 ready");
}

// ----------------------
// ACTIONS
// === LoRa
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

void sendMessage(String _message, int _nodeIdDestination, int _sendMessageCount)
{
  for (int i = 0; i < _sendMessageCount; i++)
  {
    Serial.println("Sending to RF95 Mesh Node \"" + (String)_nodeIdDestination + "\"!");
    // Send a message to a rf95_mesh_node
    // A route to the destination will be automatically discovered.

    char messageChar[_message.length() + 1];
    strcpy(messageChar, _message.c_str());

    int send_start = millis();
    int errorLog = manager->sendtoWait((uint8_t *)messageChar, sizeof(messageChar), nodeIdDestination);
    if (errorLog == RH_ROUTER_ERROR_NONE)
    {
      // It has been reliably delivered to the next node.
      // count how long sending take
      int send_end = millis();
      int send_time = send_end - send_start;
      Serial.println("sending time: " + (String)send_time);

      // Now wait for a reply from the ultimate server
      uint8_t len = sizeof(buf);
      uint8_t from;
      if (manager->recvfromAckTimeout(buf, &len, 20000, &from))
      {
        Serial.print("Got Reply from ");
        Serial.print(from, HEX);
        Serial.println(":");
        Serial.println((char *)buf);
      }
      else
      {
        Serial.println("No reply, is rf95_mesh_node1, rf95_mesh_node2 and rf95_mesh_node3 running?");
      }
    }
    else
      Serial.println("sendtoWait failed. Are the intermediate mesh nodes running?");
  }
}

void listen_lora(int listenTimeout)
{
  uint8_t len = sizeof(buf);
  uint8_t nodeIdFrom;

  if (manager->recvfromAckTimeout(buf, &len, listenTimeout, &nodeIdFrom))
  {
    Serial.print("Got Message nodeIdFrom ");
    Serial.print(nodeIdFrom, HEX);
    Serial.println(":");
    Serial.println((char *)buf);

    Serial.println("lastRssi = " + (String)driver.lastRssi());

    // Send a reply back to the originator client
    char messageResponseChar[messageResponse.length() + 1];
    strcpy(messageResponseChar, messageResponse.c_str());
    if (manager->sendtoWait((uint8_t *)messageResponseChar, sizeof(messageResponseChar), nodeIdFrom) != RH_ROUTER_ERROR_NONE)
      Serial.println("sendtoWait failed");
  }
}

// ----------------------
// MAIN
void setup()
{

  Serial.begin(9600);
  setup_lora();
  delivCount = 0;
  sess_start = millis();
  sendTimeoutStart = millis();
}

void loop()
{
  int sendInterval = 2000;
  int sendSimultCount = 1;
  if ((millis() - sendTimeoutStart) > sendInterval)
  {
    sendTimeoutStart = millis();
    sendMessage(message, nodeIdDestination, sendSimultCount);
    delivCount++;
    Serial.println((String)delivCount + "/200 sent");
  }

  listen_lora(3000);

  if (delivCount >= 200)
  {
    int sess_end = millis();
    int sess_time = sess_end - sess_start;
    Serial.println("Total time: " + (String)sess_time);
    while (1)
      ;
  }
}