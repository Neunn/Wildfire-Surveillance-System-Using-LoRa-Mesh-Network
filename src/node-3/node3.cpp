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

// ----------------------------------------------------------------------------------------------

// Define Variable ------------------------------------------------------------------------------

/* ขา PIN */
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26
/* Topology Network (จำนวนของ Mesh ทั้งหมดที่มีอยู่ใน Network)*/
#define NODE_ADDRESS 250
const uint8_t SelfAddress = NODE_ADDRESS;
/* Condition สำหรับการตัดสินใจส่ง */
float TEMPERATURE;
float TEMPERATURE_THRESHOLD = 26;
unsigned long OLDTIME = 0;
const unsigned long INTERVAL = 3 * 60 * 60 * 1000; // 3 ชั่วโมง
int16_t LISTENTIMEOUT = 16000;
/* ประกาศ Object  */
DHT DHT22_Sensor(17, DHT22);
static SSD1306Wire Screen_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
RH_RF95 RF95_Sender(LORA_SS_PIN, LORA_DIO0_PIN);
RHMesh Mesh_Manager(RF95_Sender, SelfAddress);

// ----------------------------------------------------------------------------------------------

// Custom Function ------------------------------------------------------------------------------

void SetUPLora()
{
    /* ฟังก์ชันนี้เป็นการ setup ตัวของ Module Lora sx1276 ภายใน บอร์ด ESP32 */
    if (!Mesh_Manager.init())
    {
        Serial.println();
        Serial.println("Setup LoRa Failed");
        Serial.println();
    }
    else
    {
        Serial.println();
        Serial.println("Setup Lora node " + (String)SelfAddress + " Complete");
        Serial.println();
    }
    // set ค่าความถี่ และ spreading factor
    RF95_Sender.setFrequency(923.2);
    RF95_Sender.setCADTimeout(500);
    RF95_Sender.setSignalBandwidth(125000);
    RF95_Sender.setCodingRate4(5);
    RF95_Sender.setSpreadingFactor(12);
    RF95_Sender.setTxPower(20, false);
    Serial.println();
    Serial.println("Setup RF complete");
    Serial.println();
}

void SetUPDisplay()
{
    /* ฟังก์ชันนี้เป็นการ setup ตัวของ Display OLED */
    if (!Screen_display.init())
    {
        Serial.println();
        Serial.println("Setup Display Failed");
        Serial.println();
    }
    else
    {
        Serial.println();
        Serial.println("Setup Display node " + (String)SelfAddress + " Complete");
        Serial.println();
    }
    Screen_display.clear();
    Screen_display.display();
    Screen_display.setContrast(255);

    Screen_display.drawString(0, 0, "Test Display Success.");
    Screen_display.display();
}

/* เตรียมข้อมูลที่จะส่ง */
String MESSAGE;
String MESSAGE_REPLY;
uint8_t BUFFER[RH_MESH_MAX_MESSAGE_LEN];

void ListenAndRoute(int LISTENTIMEOUT_)
{
    /* Function นี้จะเป็นการรับค่าจาก Node อื่นๆแล้วทำการ Display ว่ามาจาก Node ไหนและ Route ต่อไปยัง Node อื่นๆ */

    /* Function recvfromack จะทำหน้าที่รับข้อมูลที่มาจาก Node อื่นๆหากเป็น Node ของตัวเองจะทำการ copy ข้อมูลแล้วเก็บลงในตัวแปรที่เราใส่เข้าไป ณ ที่นี้คือ BUF แต่หากไม่ใช่ของตัว Node มันเองมันจะทำการ Route และส่งต่อไปยัง Node อื่นๆ ทำให้ Function นี้ต้องมีการรันใน loop อยู่เสมอ */

    /* The recvfromAck() function is responsible not just for receiving and delivering messages addressed to this node (or RH_BROADCAST_ADDRESS), but it is also responsible for routing other message to their next hop. This means that it is important to call recvfromAck() or recvfromAckTimeout() frequently in your main loop. recvfromAck() will return false if it receives a message but it is not for this node. ref : https://www.airspayce.com/mikem/arduino/RadioHead/classRHRouter.html#a7ac935defd2418f45a4d9f391f7e0384 */

    MESSAGE_REPLY = "Reply From Gateway : " + (String)SelfAddress;
    char MESSAGE_REPLY_CHAR_[MESSAGE_REPLY.length() + 1];
    strcpy(MESSAGE_REPLY_CHAR_, MESSAGE_REPLY.c_str());
    uint8_t BUFFER_LEN_ = sizeof(BUFFER);
    uint8_t FROM_HOP_;

    if (Mesh_Manager.recvfromAckTimeout(BUFFER, &BUFFER_LEN_, LISTENTIMEOUT_, &FROM_HOP_))
    {
        // แสดงค่า
        Serial.println();
        Serial.println("Got Message from node : " + (String)FROM_HOP_ + " Complete to Gateway");
        Serial.print("Buffer : ");
        Serial.println((char *)BUFFER);
        Serial.println("lastRssi = " + (String)RF95_Sender.lastRssi());
        Serial.println("lastSNR = " + (String)RF95_Sender.lastSNR());
        Mesh_Manager.printRoutingTable();
        Serial.println();
        Screen_display.clear();
        Screen_display.drawString(0, 10, "Recieve data from next hop");
        Screen_display.drawString(0, 20, "Success");
        Screen_display.display();

        // if (Mesh_Manager.sendtoWait((uint8_t *)MESSAGE_REPLY_CHAR_, sizeof(MESSAGE_REPLY_CHAR_), FROM_HOP_))
        // {
        //     // แสดงค่า
        //     Serial.println("Send reply to next hop");
        //     Screen_display.drawString(0, 40, "Send reply to next hop");
        //     Screen_display.display();
        // }
    }
    else
    {
        Serial.println();
        Serial.println("No Message Recieve");
        Serial.println();
        Screen_display.clear();
        Screen_display.drawString(0, 10, "No Message Recieve");
        Screen_display.display();
    }
}

// ----------------------------------------------------------------------------------------------

// Custom Function ------------------------------------------------------------------------------

void setup()
{
    Serial.begin(9600);
    SetUPLora();
    SetUPDisplay();
}

void loop()
{
    /* รับและ route ข้อมูลจาก node อื่น */
    ListenAndRoute(LISTENTIMEOUT);
}