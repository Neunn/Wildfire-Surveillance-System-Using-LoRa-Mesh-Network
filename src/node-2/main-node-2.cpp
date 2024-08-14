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

  Heltec.begin(/*DisplayEnable Enable*/ false ,
               /*Lora Disable*/ false, /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
               /*SerialEnable*/ true,
               /*PABOOST*/ true,
               /*BAND*/ 923.2E6);

  Serial.begin(9600);

  Screen_display.init();
  Screen_display.displayOn();
  Screen_display.clear(); 

  while (!Serial);

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

  // Set transmission power
  rf95.setTxPower(TX_POWER, false);

}

void loop() {
  Screen_display.clear();
  Screen_display.drawString(0, 0, "Sending to receiver...");
  Serial.println("Sending to receiver...");

  // Data to send
  const char* msg = "Hello, LoRa!";
  rf95.send((uint8_t*)msg, strlen(msg));

  // Wait until the packet is sent
  rf95.waitPacketSent();
  Serial.println("Message sent!");
  Screen_display.drawString(0, 10, "Sending to receiver...");
  Screen_display.display();

  // Wait before sending again
  delay(2000);
}

