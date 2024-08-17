// // Import header && Library -----------------------------------------------------------------------------
// #include <Arduino.h> 
// #include <SPI.h>
// #include <heltec.h>   // สำหรับควบคุม board 
// // #include <RH_RF95.h>  // สำหรับรับส่งข้อมูล
// // #include <RHMesh.h>   // สำหรับทำ Mesh
// #include <DHT_U.h>
// #include <DHT.h>      // สำหรับวัด Sensor DHT-22
// #include <MQ135.h>
// #include <MQUnifiedsensor.h>
// #include <Wire.h>
// #include <Adafruit_I2CDevice.h>
// #include <HT_SSD1306Wire.h>
// // -----------------------------------------------------------------------------------------------




// // Setup && Param Zone ----------------------------------------------------------------------------

// #define MQ7_PIN 25
// #define DHT_PIN 33 
// #define MQ135_PIN 13
// #define OLED_SDA 4
// #define OLED_SCL 15
// #define OLED_RST 16
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// #define DHT_TYPE DHT22
// // const float A_PARAM = 116.6020682;
// // const float B_PARAM = -2.769034857;
// const float VCC = 3.3;
// const float RL = 1000.0;
// const float REF_PPM = 425.55;  //https://www.co2.earth/

// static SSD1306Wire Screen_display(0x3c, 500000, OLED_SDA, OLED_SCL, GEOMETRY_128_64, OLED_RST);
// DHT DHT_Sensor(DHT_PIN, DHT_TYPE);
// MQ135 MQ135OBJ(MQ135_PIN);
// MQUnifiedsensor MQ7OBJ("Arduino UNO", 5, 12, MQ7_PIN, "MQ-7");


// // ######### Custom Function 

// float Get_CO2_Function(float humidity, float temperature){

//   /* Function สำหรับรับค่า CO2 จากโมดูล MQ-135 */
//   float rzero = MQ135OBJ.getRZero();
//   float correctedRZero = MQ135OBJ.getCorrectedRZero(temperature, humidity);
//   float resistance = MQ135OBJ.getResistance();
//   float ppm = MQ135OBJ.getPPM();
//   float CORRECTEDPPM = MQ135OBJ.getCorrectedPPM(temperature, humidity);

//   Serial.print("MQ135 RZero: ");
//   Serial.print(rzero);
//   Serial.print("\t Corrected RZero: ");
//   Serial.print(correctedRZero);
//   Serial.print("\t Resistance: ");
//   Serial.print(resistance);
//   Serial.print("\t PPM: ");
//   Serial.print(ppm);
//   Serial.print("ppm");
//   Serial.print("\t Corrected PPM: ");
//   Serial.print(CORRECTEDPPM);
//   Serial.println("ppm");
//   return CORRECTEDPPM;
// }

// float Get_CO_Function(){
//   MQ7OBJ.update();
//   float PPMCO = MQ7OBJ.readSensor();
//   Serial.println("CO PPM = " + String(PPMCO));
//   return PPMCO;
// }

// // ######### Setup Zone

// void setup(){
//   Heltec.begin(/*DisplayEnable Enable*/ false ,
//                /*Lora Disable*/ false, /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
//                /*SerialEnable*/ true,
//                /*PABOOST*/ true,
//                /*BAND*/ 923.2E6);
//   Serial.begin(9600);
//   DHT_Sensor.begin();

//   MQ7OBJ.init();
//   MQ7OBJ.setRegressionMethod(1);
//   MQ7OBJ.setA(99.042); 
//   MQ7OBJ.setB(-1.518);

//   float calcR0 = 0;
//   for (int i = 1; i <= 10; i++) {
//     MQ7OBJ.update();
//     calcR0 += MQ7OBJ.calibrate(10.0);
//     delay(1000);
//   }


//       // Screen setup
//   Screen_display.init();
//   Screen_display.displayOn();
//   Screen_display.clear();       // Clear หน้าจอ
//   Serial.println("Heltec WiFi LoRa 32 (V2) Starting....");
  
//       // Pin Setup
//   pinMode(MQ7_PIN, INPUT);
//   pinMode(MQ135_PIN, INPUT);
  
// }
// // -----------------------------------------------------------------------------------------------


// // Main Zone ------------------------------------------------------------------------------------
// void loop(){

//   delay(5000);
//   Screen_display.clear();


//   float humidity = DHT_Sensor.readHumidity();
//   float temp = DHT_Sensor.readTemperature();
//   float PPMCO2 = Get_CO2_Function(humidity, temp);
//   float PPMCO = Get_CO_Function();


//   Serial.print("Humidity = ");
//   Serial.println(humidity);
//   Serial.print("Temperature = ");
//   Serial.println(temp);

//       // แสดงหน้า Display
//   Screen_display.setTextAlignment(TEXT_ALIGN_LEFT);
//   Screen_display.drawString(0, 0, "Humidity = " + String(humidity));
//   Screen_display.drawString(0, 10, "Temperature = " + String(temp));
//   Screen_display.drawString(0, 20, "CO2 = " + String(PPMCO2) + " ppm");
//   Screen_display.drawString(0, 30, "CO = " + String(PPMCO) + " ppm");
//   Screen_display.display();
  


//   // Serial.println("อ่านค่าความชื้น")
// }
// // -----------------------------------------------------------------------------------------------









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
#include <esp_task_wdt.h>
// ----------------------------------------------------------------------------------------------



// Define Variable ------------------------------------------------------------------------------
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26
#define SENDING_MODE 0      /*Mode สำหรับการส่งข้อความไปยัง Node อื่นๆหรือ Gateway*/
#define RECEIVING_MODE 1    /*Node จะทำการรอรับข้อความจาก Node อื่นๆ*/
     /* Topology Network (จำนวนของ Mesh ทั้งหมดที่มีอยู่ใน Network)*/
#define NODE1_ADDRESS 1
#define NODE2_ADDRESS 2
#define NODE3_ADDRESS 3 // ทำเป็น Gateway จำลองรอ Gateway เสร็จก่อน
const uint8_t SelfAddress = NODE1_ADDRESS;
const uint8_t GatewayAddress = NODE3_ADDRESS;
std::string PACKAGE_SND = String("This is Package from " + String(SelfAddress)).c_str(); 
std::string PACKAGE_RCV ; /*เก็บข้อความที่รับมาจาก Node อื่นๆ*/

static SSD1306Wire Screen_display(0x3c, 
                                  500000,
                                  SDA_OLED,
                                  SCL_OLED,
                                  GEOMETRY_128_64,
                                  RST_OLED);
RH_RF95 RF95_Sender(LORA_SS_PIN, LORA_DIO0_PIN);
// RHMesh Mesh_Manager()
int counter = 1;
// ----------------------------------------------------------------------------------------------



// Custom Function ------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------



// Setup Function -------------------------------------------------------------------------------
void setup(){
    Heltec.begin(true, /*DisplayEnable*/
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
}
// ----------------------------------------------------------------------------------------------


// Main Function --------------------------------------------------------------------------------
void loop(){
    // Serial.println("Broadcasting...");
    // Serial.println("Counter " + String(counter));
    // Screen_display.clear();
    // Screen_display.drawString(0, 0, "Broadcasting...");
    // Screen_display.drawString(0, 10, "Counter " + String(counter));


      
    
    if (!RF95_Sender.isChannelActive()){
        Screen_display.drawString(0, 0, "Channel is active. Waiting...");
    } else {
        
        Screen_display.drawString(0, 0, "Channel is free. Sending data...");
        RF95_Sender.waitPacketSent();
    }

    counter++;
    delay(1000);
}