// Import header && Library ---------------------------------------------------------------------
#include <Arduino.h> 
#include <SPI.h>
#include <Wire.h>
#include <heltec.h>
#include <RH_RF95.h>
#include <HT_SSD1306Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
// ----------------------------------------------------------------------------------------------



// Define Variable ------------------------------------------------------------------------------
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26
static SSD1306Wire Screen_display(0x3c, 
                                  500000,
                                  SDA_OLED,
                                  SCL_OLED,
                                  GEOMETRY_128_64,
                                  RST_OLED);
RH_RF95 RF95_Sender(LORA_SS_PIN, LORA_DIO0_PIN);
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
    Serial.println("Broadcasting...");
    Screen_display.clear();
    Screen_display.drawString(0, 0, "Broadcasting...");
    Screen_display.drawString(0, 10, "Counter " + String(counter));
    String Radio_Packet = "Message Counter " + String(counter) + " From Device 1";
      
    
    if (!RF95_Sender.isChannelActive()){
        Screen_display.drawString(0, 30, "Channel is active. Waiting...");
    } else {
        RF95_Sender.send((uint8_t*)Radio_Packet.c_str(), Radio_Packet.length()+1);  
        Screen_display.drawString(0, 30, "Channel is free. Sending data...");
        RF95_Sender.waitPacketSent();
    }   

    counter++;
    delay(1000);
}