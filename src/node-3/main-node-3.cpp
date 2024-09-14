


// ----------------------
// PRE-REQUISITES
// === LoRa
#include <Arduino.h>
#include <heltec.h>
#include <SPI.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>


// ESP32
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 923.2

int nodeIdSelf = 4;
 
// Singleton instance of the radio driver
RH_RF95 driver(LORA_SS_PIN, LORA_DIO0_PIN);

// Class to manage message delivery and receipt, using the driver declared above
RHMesh *manager;

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

void listen_lora() {
  uint8_t len = sizeof(buf);
  uint8_t nodeIdFrom;

  if (manager->recvfromAck(buf, &len, &nodeIdFrom))
  {
    count++;
    Serial.println((String) count + "/200 received");
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

// ----------------------
// MAIN
void setup() {
    Heltec.begin(false,                /*DisplayEnable*/
                 false, /*LoRaEnable*/ /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
                 true,                 /*SerialEnable*/
                 true,                 /*PABOOST*/
                 923.2E6 /*Band*/);
    Serial.begin(9600);
    setup_lora();
}

void loop()
{
  listen_lora();
}