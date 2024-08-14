#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <HT_SSD1306Wire.h>
#include <RH_RF95.h>
#include <heltec.h>

// Define LoRa module pins
#define RFM95_CS  18
#define RFM95_RST 14
#define RFM95_INT 26
#define RF95_FREQ 923.2E6 
#define TX_POWER 14    


RH_RF95 rf95(RFM95_CS, RFM95_INT);
static SSD1306Wire Screen_display(0x3c, 500000, 4, 15, GEOMETRY_128_64, 16);

void setup() {
  Serial.begin(9600);
  Heltec.begin(/*DisplayEnable Enable*/ false ,
               /*Lora Disable*/ false, /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
               /*SerialEnable*/ true,
               /*PABOOST*/ true,
               /*BAND*/ 923.2E6);

  Screen_display.init();
  Screen_display.displayOn();
  Screen_display.clear(); 

  while (!Serial);

  // Initialize LoRa module
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("LoRa init failed");
    while (1);
  }
  Serial.println("LoRa init succeeded");

  // Set frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("Set frequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Optionally set additional parameters (e.g., bandwidth, coding rate)
}

void loop() {
  Screen_display.clear();
  if (rf95.available()) {
    // Buffer to hold received message
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      Serial.print("Received: ");
      Serial.println((char*)buf);
      Screen_display.drawString(0, 0, (char*)buf);
      
    } else {
      Serial.println("Receive failed");
      Screen_display.drawString(0, 0, "Receive failed");
    }
  }
  Screen_display.display();
  delay(2000);  
}