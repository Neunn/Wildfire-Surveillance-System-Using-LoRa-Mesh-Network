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

// Import header && Library -----------------------------------------------------------------------------
#include <Arduino.h> 
#include <SPI.h>
#include <Wire.h>
#include <heltec.h>
// -----------------------------------------------------------------------------------------------