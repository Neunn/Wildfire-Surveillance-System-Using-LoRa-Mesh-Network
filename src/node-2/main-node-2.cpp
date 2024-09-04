
// Import header && Library ---------------------------------------------------------------------
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
// ----------------------------------------------------------------------------------------------



// Define Variable ------------------------------------------------------------------------------
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26
     /* Topology Network (จำนวนของ Mesh ทั้งหมดที่มีอยู่ใน Network)*/
#define NODE1_ADDRESS 1
#define NODE2_ADDRESS 2
#define NODE3_ADDRESS 3 // ทำเป็น Gateway จำลองรอ Gateway เสร็จก่อน
// boolean SENDING_MODE = true;
const uint8_t SelfAddress = NODE3_ADDRESS;
const uint8_t GatewayAddress = NODE3_ADDRESS;
     /*Condition*/
float TEMPERATURE;
int COUNT_PER_DAY = 0;
unsigned long OLDTIME = 0;
unsigned long CYCLE_TIME = 14400000;
     /*Obejct*/
MQUnifiedsensor MQ135_Sensor("esp32", 3.3, 12, 2, "MQ-135");
MQUnifiedsensor MQ7_Sensor("esp32", 3.3, 12, 13, "MQ-7");
DHT DHT22_Sensor(17, DHT22);
static SSD1306Wire Screen_display(0x3c, 
                                  500000,
                                  SDA_OLED,
                                  SCL_OLED,
                                  GEOMETRY_128_64,
                                  RST_OLED);
RH_RF95 RF95_Sender(LORA_SS_PIN, LORA_DIO0_PIN);
RHMesh Mesh_Manager(RF95_Sender, SelfAddress);
// ----------------------------------------------------------------------------------------------



void setup(){
    Heltec.begin(false, /*DisplayEnable*/
                 false, /*LoRaEnable*/ /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
                 true,  /*SerialEnable*/
                 true, /*PABOOST*/
                 923.2E6 /*Band*/);
    Serial.begin(9600);
    
    /*RF95 setting*/
    RF95_Sender.init();
    RF95_Sender.setFrequency(923.2);
    RF95_Sender.setSpreadingFactor(12);
    RF95_Sender.setSignalBandwidth(125E3);
    // RF95_Sender.setTxPower(23); //ค่อยมาคำนวนอีกที

    /*Mesh setting*/
    // Mesh_Manager.setTimeout(10000);
    
    /*ตั้งค่าหน้าจอ OLED*/
    Screen_display.init();
    Screen_display.clear();
    Screen_display.display();
    Screen_display.setContrast(255);
}


// Main Function --------------------------------------------------------------------------------
void loop(){
  uint8_t data[] = "And hello back to you from server1";
  // Dont put this on the stack:
  uint8_t buf[40] = {0};
  uint8_t len = sizeof(buf);
  uint8_t from;
  Mesh_Manager.printRoutingTable();
  if (Mesh_Manager.recvfromAck(buf, &len, &from)){
    Serial.print("got request from : 0x");
    Serial.print(from, HEX);
    Serial.print(": ");
    Serial.println((char*)buf);
  


    // Send a reply back to the originator client
    if (Mesh_Manager.sendtoWait(data, sizeof(data), from) == RH_ROUTER_ERROR_NONE){
        Serial.println("Sendtowait from " + String((char*)SelfAddress));
    } else {
        Serial.println("Sendtowait Failed");
    }
  }
}