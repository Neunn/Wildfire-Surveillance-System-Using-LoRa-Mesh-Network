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
#include <MQUnifiedsensor.h>

#define RF95_FREQ 923.2

int nodeIdSelf = 4;

// Singleton instance of the radio driver
RH_RF95 driver(18, 26);

// Class to manage message delivery and receipt, using the driver declared above
RHMesh *manager;

String messageResponse = "Alert Received!";

int sess_start, sendTimeoutStart;

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

void listen_lora(int listenTimeout)
{
  uint8_t len = sizeof(buf);
  uint8_t nodeIdFrom;

  if (listenTimeout == 0)
  {
    // continuous listening mode
    if (manager->recvfromAck(buf, &len, &nodeIdFrom))
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
  else
  {
    // timeout listening mode
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
}

// ----------------------
// MAIN
void setup()
{
  Serial.begin(9600);
  setup_lora();
  sess_start = millis();
  sendTimeoutStart = millis();
}

void loop()
{
  listen_lora(3000);
}
