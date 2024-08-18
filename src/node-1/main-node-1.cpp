/*ลองเขียนใหม่*/

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
#define SENDING_MODE 0      /*Mode สำหรับการส่งข้อความไปยัง Node อื่นๆหรือ Gateway*/
#define RECEIVING_MODE 1    /*Node จะทำการรอรับข้อความจาก Node อื่นๆ*/
uint8_t MODE = RECEIVING_MODE;
     /* Topology Network (จำนวนของ Mesh ทั้งหมดที่มีอยู่ใน Network)*/
#define NODE1_ADDRESS 1
#define NODE2_ADDRESS 2
#define NODE3_ADDRESS 3 // ทำเป็น Gateway จำลองรอ Gateway เสร็จก่อน
const uint8_t SelfAddress = NODE1_ADDRESS;
const uint8_t GatewayAddress = NODE3_ADDRESS;
float TEMPERATURE;
int COUNT_PER_DAY = 0;
uint8_t PACKAGE_RCV_BUF[RH_MESH_MAX_MESSAGE_LEN];
// std::string PACKAGE_SND = String("This is Package from " + String(SelfAddress)).c_str(); 
// std::string PACKAGE_RCV ; /*เก็บข้อความที่รับมาจาก Node อื่นๆ*/

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



// Custom Function ------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------



// Setup Function -------------------------------------------------------------------------------
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
    RF95_Sender.setTxPower(23); //ค่อยมาคำนวนอีกที
    
    /*ตั้งค่าหน้าจอ OLED*/
    Screen_display.init();
    Screen_display.clear();
    Screen_display.display();
    Screen_display.setContrast(255);



    /*DHT-22 Sensor Begin*/
    DHT22_Sensor.begin();

    // /*MQ Sensor*/
    // MQ135_Sensor.setRegressionMethod(1); //_PPM =  a*ratio^b
    // MQ135_Sensor.setA(110.47);
    // MQ135_Sensor.setB(-2.862); // Configure the equation to to calculate CO2 concentration
    // MQ135_Sensor.init();

    // MQ7_Sensor.setRegressionMethod(1); //_PPM =  a*ratio^b
    // MQ7_Sensor.setA(99.042); 
    // MQ7_Sensor.setB(-1.518); // Configure the equation to calculate CO concentration value
    // MQ7_Sensor.init();

    // /*Pre-heat 20 วิ && Calibration*/
    // Screen_display.clear();
    // Screen_display.drawString(0, 0, "Pre-heated & Calibration 20 sec pls wait...");
    // Screen_display.display();
    
    // unsigned long TIME_PERIOD = 20000.0;
    // unsigned long OLD_TIME = 0.0;
    // float calcR0_MQ135 = 0.0;
    // float calcR0_MQ7 = 0.0;
    // for (int i = 1; i <= 10; i++){
    //     MQ135_Sensor.update();
    //     MQ7_Sensor.update();
    //     calcR0_MQ135 += MQ135_Sensor.calibrate(3.6); /*ref : https://www.electronicoscaldas.com/datasheet/MQ-135_Hanwei.pdf*/
    //     calcR0_MQ7 += MQ7_Sensor.calibrate(27.5); /*ref : https://www.sparkfun.com/datasheets/Sensors/Biometric/MQ-7.pdf*/
    // }
    // MQ135_Sensor.setR0(calcR0_MQ135/10);
    // MQ7_Sensor.setR0(calcR0_MQ7/10);
    // MQ135_Sensor.serialDebug(true);
    // MQ7_Sensor.serialDebug(true);

    // OLD_TIME = millis();
    // while (millis() - OLD_TIME < TIME_PERIOD){
    //     MQ7_Sensor.update();
    //     MQ7_Sensor.readSensor();
    //     Serial.println("#################################");
    //     MQ7_Sensor.serialDebug();
    //     Serial.println("#################################");
    //     MQ135_Sensor.update();
    //     MQ135_Sensor.readSensor();
    //     Serial.println("#################################");
    //     MQ135_Sensor.serialDebug();
    //     Serial.println("#################################");

    // }
    // Screen_display.clear();
    // Screen_display.drawString(0, 0, "Pre-heated & Calibration successful");
    // Screen_display.display();
}
// ----------------------------------------------------------------------------------------------


// Main Function --------------------------------------------------------------------------------
void loop(){

    TEMPERATURE = DHT22_Sensor.readTemperature();

    if (TEMPERATURE >= 27){
        while(RF95_Sender.isChannelActive()){
            Screen_display.clear();
            Screen_display.drawString(0, 0, "Channel is Busy...");
            Screen_display.display();
            delay(500);
        }
        
        /*เตรียมข้อมูลที่จะส่ง*/
        char DATA[RH_MAX_MESSAGE_LEN];
        snprintf(DATA, sizeof(DATA), "Temp = %.2f : FROM Node =  %d", TEMPERATURE, SelfAddress);
        
        /*ส่งข้อมูลไปยัง Gateway*/
        boolean SENDING = true;
        while(SENDING){
            if (Mesh_Manager.sendtoWait((uint8_t*)DATA, sizeof(DATA), NODE3_ADDRESS) == RH_ROUTER_ERROR_NONE){
                Serial.println("Message sent to Gateway");
                Screen_display.clear();
                Screen_display.drawString(0, 0, "Sending Message to GW");
                Screen_display.display();
                SENDING = false;
            }
            else{
                Serial.println("Message sending failed");
                Screen_display.clear();
                Screen_display.drawString(0, 0, "Message sending failed");
                Screen_display.drawString(0, 10, "Resending.....");
                Screen_display.display();
                SENDING = true;
            }
        }

    } else{
        
    }

    // if (!RF95_Sender.isChannelActive()){
    //     Screen_display.drawString(0, 0, "Channel is active. Waiting...");
    // } else {
        
    //     Screen_display.drawString(0, 0, "Channel is free. Sending data...");
    //     RF95_Sender.waitPacketSent();
    // }


    // Serial.println("MQ135_Sensor");
    // MQ135_Sensor.update();
    // MQ135_Sensor.readSensor();
    // MQ135_Sensor.serialDebug();

    // Serial.println("MQ7_Sensor");
    // MQ7_Sensor.update();
    // MQ7_Sensor.readSensor();
    // MQ7_Sensor.serialDebug();



    delay(1000);
}