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

/* ขา PIN */
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26

/* Topology Network (จำนวนของ Mesh ทั้งหมดที่มีอยู่ใน Network)*/
#define NODE1_ADDRESS 1
#define NODE2_ADDRESS 2
#define NODE3_ADDRESS 3 // ทำเป็น Gateway จำลองรอ Gateway เสร็จก่อน
#define N_NODES 10
uint8_t ROUTES[N_NODES];
int16_t RSSI[N_NODES];
const uint8_t SelfAddress = NODE1_ADDRESS;
const uint8_t GatewayAddress = NODE3_ADDRESS;

/*Condition*/
float TEMPERATURE;
int COUNT_PER_DAY = 0;
unsigned long OLDTIME = 0;
unsigned long CYCLE_TIME = 14400000;

/* ประกาศ Object  */
MQUnifiedsensor MQ135_Sensor("esp32", 3.3, 12, 2, "MQ-135");
MQUnifiedsensor MQ7_Sensor("esp32", 3.3, 12, 13, "MQ-7");
DHT DHT22_Sensor(17, DHT22);
static SSD1306Wire Screen_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
RH_RF95 RF95_Sender(LORA_SS_PIN, LORA_DIO0_PIN);
RHMesh Mesh_Manager(RF95_Sender, SelfAddress);


// ----------------------------------------------------------------------------------------------





// Custom Function ------------------------------------------------------------------------------

float readDHTTemperature() {
    /* มีการ Smapling ค่าเป็นจำนวนสิบค่า แล้วหาค่าเฉลี่ยออกมา */

    float TEMP_C = 0;
    float TOTAL_VALUES = 0; 
    for (int i = 0; i < 10; i++){
        TEMP_C = DHT22_Sensor.readTemperature();
        TOTAL_VALUES += TEMP_C;
        delay(125);
    }
    float AVERAGE_C = TOTAL_VALUES / 10;
    return AVERAGE_C;

}

float SendDataToGateway(float TEMP_C){
    
}

// ----------------------------------------------------------------------------------------------






// Setup Function -------------------------------------------------------------------------------

void setup(){

    Heltec.begin(false,                /*DisplayEnable*/
                 false, /*LoRaEnable*/ /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
                 true,                 /*SerialEnable*/
                 true,                 /*PABOOST*/
                 923.2E6 /*Band*/);
    Serial.begin(9600);

    /*Mesh Manager*/
    Mesh_Manager.init();

    /*RF95 setting*/
    RF95_Sender.init();
    RF95_Sender.setFrequency(923.2); /*ย่านความถี่ UL ที่ กสทช. แนะนำและอยู่ในช่วงที่กฏหมายกำหนด*/
    RF95_Sender.setSpreadingFactor(12);  /*ตั้งไว้ 12 เพราะต้องการระยะที่ไกลแต่ไม่ได้ต้องการความเร็วที่สูงมาก*/
    RF95_Sender.setSignalBandwidth(125E3); 
    RF95_Sender.setTxPower(23); // ค่อยมาคำนวนอีกที

    /*ตั้งค่าหน้าจอ OLED*/
    Screen_display.init();
    Screen_display.clear();
    Screen_display.display();
    Screen_display.setContrast(255);

    /* DHT22 setup */
    DHT22_Sensor.begin();
}

// ----------------------------------------------------------------------------------------------






// Loop Function --------------------------------------------------------------------------------
void loop(){

}


// ----------------------------------------------------------------------------------------------