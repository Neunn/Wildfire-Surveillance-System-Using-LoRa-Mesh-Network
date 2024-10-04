// // Import header && Library ---------------------------------------------------------------------

// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <heltec.h>
// #include <RH_RF95.h>
// #include <RHMesh.h>
// #include <HT_SSD1306Wire.h>
// #include <Adafruit_I2CDevice.h>
// #include <Adafruit_Sensor.h>
// #include <DHT.h>
// #include <MQUnifiedsensor.h>

// // ----------------------------------------------------------------------------------------------

// // Define Variable ------------------------------------------------------------------------------

// /* ขา PIN */
// #define LORA_SS_PIN 18
// #define LORA_DIO0_PIN 26

// /* Topology Network (จำนวนของ Mesh ทั้งหมดที่มีอยู่ใน Network)*/
// #define NODE1_ADDRESS 1
// #define NODE2_ADDRESS 2
// #define NODE3_ADDRESS 3 // ทำเป็น Gateway จำลองรอ Gateway เสร็จก่อน
// const uint8_t SelfAddress = NODE2_ADDRESS;
// const uint8_t GatewayAddress = NODE3_ADDRESS;

// /*Condition*/
// float TEMPERATURE;
// float TEMPERATURE_THRESHOLD = 45;
// unsigned long OLDTIME = 0;
// const unsigned long INTERVAL = 3 * 60 * 60 * 1000; // 3 ชั่วโมง

// /* ประกาศ Object  */
// MQUnifiedsensor MQ135_Sensor("esp32", 3.3, 12, 2, "MQ-135");
// MQUnifiedsensor MQ7_Sensor("esp32", 3.3, 12, 13, "MQ-7");
// DHT DHT22_Sensor(17, DHT22);
// static SSD1306Wire Screen_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
// RH_RF95 RF95_Sender(LORA_SS_PIN, LORA_DIO0_PIN);
// RHMesh Mesh_Manager(RF95_Sender, SelfAddress);

// // ----------------------------------------------------------------------------------------------

// // Custom Function ------------------------------------------------------------------------------

// float readDHTTemperature()
// {
//     /* มีการ Smapling ค่าเป็นจำนวนสิบค่า แล้วหาค่าเฉลี่ยออกมา */

//     float TEMP_C = 0;
//     float TOTAL_VALUES = 0;
//     for (int i = 0; i < 10; i++)
//     {
//         TEMP_C = DHT22_Sensor.readTemperature();
//         TOTAL_VALUES += TEMP_C;
//         delay(125);
//     }
//     float AVERAGE_C = TOTAL_VALUES / 10;
//     Screen_display.clear();
//     Screen_display.drawString(0, 0, "Node : " + String(SelfAddress));
//     Screen_display.drawString(0, 10, "Temperature: " + String(AVERAGE_C));
//     Screen_display.display();
//     Serial.println("Temerature : " + String(AVERAGE_C));
//     return AVERAGE_C;
// }

// void SendDataToGateway(float TEMPERATURE)
// {
//     /* Function ในการส่งข้อมูลนี้ไปยัง Gateway โดยจะมีการค้นหาเส้นทางอัตโนมัติหรือ Automatic Routing นั่นเอง และเมื่อส่งข้อความสำเร็จจะมี message ส่งคืนกลับมาที่ node นี้นั่นเอง*/

//     /* เตรียมข้อมูลที่จะส่ง */
//     char MESSAGE[40];
//     uint8_t REPLY[40];
//     uint8_t REPLY_LEN = sizeof(REPLY);
//     uint8_t FROM;
//     snprintf(MESSAGE, sizeof(MESSAGE), "Temp = %.2f C", TEMPERATURE);

//     /* ส่งข้อมูล */
//     if (Mesh_Manager.sendtoWait((uint8_t *)MESSAGE, sizeof(MESSAGE), GatewayAddress))
//     {
//         Screen_display.clear();
//         Screen_display.drawString(0, 0, "Node : " + SelfAddress);
//         Screen_display.drawString(0, 10, "Temperature : " + String(TEMPERATURE));
//         Screen_display.drawString(0, 20, "Send Data to Gateway....");
//         Screen_display.display();
//         Serial.println("Temperature : " + String(TEMPERATURE));
//         Serial.println("Send Data to Gateway");
//         delay(500);

//         /* แสดงค่า Routing table */
//         Serial.println("############ Routing Table ############");
//         Serial.println();
//         Mesh_Manager.printRoutingTable();
//         Serial.println();
//         Serial.println("#######################################");
//     }
//     else
//     {
//         Screen_display.clear();
//         Screen_display.drawString(0, 0, "Node : " + SelfAddress);
//         Screen_display.drawString(0, 10, "Temperature : " + String(TEMPERATURE));
//         Screen_display.drawString(0, 20, "Send Data to Gateway Fail!!!!");
//         Screen_display.drawString(0, 30, "Retry again....");
//         Screen_display.display();

//         Serial.println("Sendtowait Fail");
//         delay(500);
//     }
// }

// void RecieveAndRoute()
// {
//     /* Function นี้จะเป็นการรับค่าจาก Node อื่นๆแล้วทำการ Display ว่ามาจาก Node ไหนและ Route ต่อไปยัง Node อื่นๆ */

//     /* Function recvfromack จะทำหน้าที่รับข้อมูลที่มาจาก Node อื่นๆหากเป็น Node ของตัวเองจะทำการ copy ข้อมูลแล้วเก็บลงในตัวแปรที่เราใส่เข้าไป ณ ที่นี้คือ BUF แต่หากไม่ใช่ของตัว Node มันเองมันจะทำการ Route และส่งต่อไปยัง Node อื่นๆ ทำให้ Function นี้ต้องมีการรันใน loop อยู่เสมอ */

//     /* The recvfromAck() function is responsible not just for receiving and delivering messages addressed to this node (or RH_BROADCAST_ADDRESS), but it is also responsible for routing other message to their next hop. This means that it is important to call recvfromAck() or recvfromAckTimeout() frequently in your main loop. recvfromAck() will return false if it receives a message but it is not for this node. ref : https://www.airspayce.com/mikem/arduino/RadioHead/classRHRouter.html#a7ac935defd2418f45a4d9f391f7e0384*/

//     /* Create BUFFER */
//     uint8_t BUF[40];
//     uint8_t BUF_LEN = sizeof(BUF);
//     uint8_t FROM;
//     char MESSAGE[40];
//     snprintf(MESSAGE, sizeof(MESSAGE), "Reply from hop %u", SelfAddress);

//     /* รับข้อความ หรือ route ข้อความไปยัง Node ถัดไป */
//     if (Mesh_Manager.recvfromAck(BUF, &BUF_LEN, &FROM))
//     {
//         Screen_display.clear();
//         Screen_display.drawString(0, 0, "Node : " + String(SelfAddress));
//         Screen_display.drawString(0, 10, "Data Received from Node : " + String(FROM, HEX));
//         Screen_display.drawString(0, 20, "Data : " + String((char *)BUF));
//         Screen_display.drawString(0, 30, "Complete To Gateway");
//         Screen_display.display();
//         Serial.println();
//         Serial.println("Node : " + String(SelfAddress));
//         Serial.println("Data Received from Node : " + String(FROM, HEX));
//         Serial.println("Data : " + String((char *)BUF));
//         Serial.println("Complete To Gateway");
//         Serial.println();
//         delay(500);

//         /* ส่ง Reply กลับไปยัง Node ที่รับมา */
//         if (Mesh_Manager.sendtoWait((uint8_t *)MESSAGE, strlen(MESSAGE), FROM))
//         {
//             Screen_display.clear();
//             Screen_display.drawString(0, 0, "Node : " + String(SelfAddress));
//             Screen_display.drawString(0, 10, "Send reply back to : " + String(FROM, HEX));
//             Screen_display.display();
//             Serial.println();
//             Serial.println("Send reply back to : " + String(FROM, HEX));
//             Serial.println();

//             delay(500);
//         }
//         else
//         {
//             Screen_display.clear();
//             Screen_display.drawString(0, 0, "Node : " + String(SelfAddress));
//             Screen_display.drawString(0, 10, "Error reply fail");
//             Screen_display.display();
//             Serial.println();
//             Serial.println("Node : " + String(SelfAddress));
//             Serial.println("Error reply fail");
//             Serial.println();
//         }
//     }
//     else
//     {
//         Serial.println();
//         Serial.println("No Message from other node or route failed");
//         Serial.println();
//         Screen_display.clear();
//         Screen_display.drawString(0, 0, "Node : " + String(SelfAddress));
//         Screen_display.drawString(0, 10, "No Message come");
//         Screen_display.display();
//         delay(500);
//     }
// }

// // ----------------------------------------------------------------------------------------------

// // Setup Function -------------------------------------------------------------------------------

// void setup()
// {

//     Heltec.begin(false,                /*DisplayEnable*/
//                  false, /*LoRaEnable*/ /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
//                  true,                 /*SerialEnable*/
//                  true,                 /*PABOOST*/
//                  923.2E6 /*Band*/);
//     Serial.begin(9600);

//     /*Mesh Manager*/
//     Mesh_Manager.init();

//     /*RF95 setting*/
//     RF95_Sender.init();
//     RF95_Sender.setFrequency(923.2);    /*ย่านความถี่ UL ที่ กสทช. แนะนำและอยู่ในช่วงที่กฏหมายกำหนด*/
//     RF95_Sender.setSpreadingFactor(12); /*ตั้งไว้ 12 เพราะต้องการระยะที่ไกลแต่ไม่ได้ต้องการความเร็วที่สูงมาก*/
//     RF95_Sender.setSignalBandwidth(125E3);
//     RF95_Sender.setTxPower(23); // ค่อยมาคำนวนอีกที

//     /*ตั้งค่าหน้าจอ OLED*/
//     Screen_display.init();
//     Screen_display.clear();
//     Screen_display.display();
//     Screen_display.setContrast(255);

//     /* DHT22 setup */
//     DHT22_Sensor.begin();
// }

// // ----------------------------------------------------------------------------------------------

// // Loop Function --------------------------------------------------------------------------------

// void loop()
// {
//     /* ต้องการให้ตัว endnode ตัวนี้มีการส่งข้อมูลทุกๆ 3 ชั่วโมงไปหา gateway แต่หากมีอุณหภูมิสูงกว่าเกินที่กำหนดให้ทำการส่งข้อมูลไปเลยโดยไม่ต้องรอครบ 3 ชั่วโมง แล้วส่งครั้งเดียวจะได้ไม่เกิดการ flood message */

//     /* อ่านค่าอุณหภูมิ */
//     float TEMPERATURE = readDHTTemperature();

//     /* จับเวลา */
//     unsigned long CURRENTIME = millis();
//     /* เช็คหากครบสามชั่วโมงให้ส่งข้อมูลเลย */
//     if (CURRENTIME - OLDTIME >= INTERVAL)
//     {
//         OLDTIME = CURRENTIME;
//         SendDataToGateway(TEMPERATURE);
//     }

//     /* เช็คหากเงื่อนบางประการ ณ ตอนนี้กำหนดให้ อุณหภูมิ สูงกว่าค่า Threshold ให้ส่งเลยโดยไม่ต้องรอให้ครบสามชั่วโมง */
//     // while (TEMPERATURE >= TEMPERATURE_THRESHOLD){
//     //     SendDataToGateway(TEMPERATURE);
//     //     delay(100);
//     // }

//     SendDataToGateway(TEMPERATURE);
//     Serial.println();
//     Serial.println("Time = " + String(OLDTIME));
//     Serial.println();

//     /* รับและ route ข้อมูลจาก node อื่น */
//     delay(5000);
//     RecieveAndRoute();
// }

// // ----------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <SPI.h>
#include <RHMesh.h>
#include <RH_RF95.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <heltec.h>

// ESP32
#define LORA_SS_PIN 18
#define LORA_DIO0_PIN 26

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 923.2

int nodeIdSelf = 2;
int nodeIdDestination = 3;
int startTimer;
int sendInterval = 2000;

// Singleton instance of the radio driver
RH_RF95 driver(LORA_SS_PIN, LORA_DIO0_PIN);

// Class to manage message delivery and receipt, using the driver declared above
RHMesh *manager;

String message;
String messageResponse = "Alert Received!";

int count = 0;

// ----------------------
// SETUP
// === LoRa
void setup_lora()
{
  manager = new RHMesh(driver, nodeIdSelf);

  if (!manager->init())
  {
    Serial.println(F("init failed"));
  }
  else
  {
    Serial.println("Mesh Node \"" + (String)nodeIdSelf + "\" Up and Running!");
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

void sendMessage(String _message, int _nodeIdDestination, int _sendMessageCount)
{
  for (int i = 0; i < _sendMessageCount; i++)
  {
    Serial.println("Sending to RF95 Mesh Node \"" + (String)_nodeIdDestination + "\"!");
    // Send a message to a rf95_mesh_node
    // A route to the destination will be automatically discovered.

    char messageChar[_message.length() + 1];
    strcpy(messageChar, _message.c_str());

    int errorLog = manager->sendtoWait((uint8_t *)messageChar, sizeof(messageChar), nodeIdDestination);
    if (errorLog == RH_ROUTER_ERROR_NONE)
    {
      // It has been reliably delivered to the next node.
      // Now wait for a reply from the ultimate server
      uint8_t len = sizeof(buf);
      uint8_t from;
      if (manager->recvfromAckTimeout(buf, &len, 3000, &from))
      {
        Serial.print("Got Reply from ");
        Serial.print(from, HEX);
        Serial.println(":");
        Serial.println((char *)buf);
      }
      else
      {
        Serial.println("No reply, is rf95_mesh_node1, rf95_mesh_node2 and rf95_mesh_node3 running?");
      }
    }
    else
      Serial.println("sendtoWait failed. Are the intermediate mesh nodes running?");
  }
}

void listen_lora()
{
  uint8_t len = sizeof(buf);
  uint8_t nodeIdFrom;

  if (manager->recvfromAck(buf, &len, &nodeIdFrom))
  {
    Serial.print("Got Message nodeIdFrom ");
    Serial.print(nodeIdFrom, HEX);
    Serial.println(":");
    Serial.println((char *)buf);

    Serial.println("lastRssi = " + (String)driver.lastRssi());

    // Send a reply back to the originator client
    char messageResponseChar[messageResponse.length() + 1];
    strcpy(messageResponseChar, messageResponse.c_str());
    if (manager->sendtoWait((uint8_t *)messageResponseChar, sizeof(messageResponseChar), nodeIdFrom) != RH_ROUTER_ERROR_NONE)
      Serial.println("sendtoWait failed");
  }
}

void setup()
{
  Heltec.begin(false,                /*DisplayEnable*/
               false, /*LoRaEnable*/ /*ต้องปิดเพราะไม่งั้นจะไม่ conflix กันกับ RH_RF95.h */
               true,                 /*SerialEnable*/
               true,                 /*PABOOST*/
               923.2E6 /*Band*/);
  Serial.begin(9600);
  setup_lora();
  startTimer = millis();
  message = "Hello! from node " + (String)nodeIdSelf + ", to node " + (String)nodeIdDestination;
}

void loop()
{
  if (millis() - startTimer > sendInterval)
  {
    startTimer = millis();
    sendMessage(message, nodeIdDestination, 1);
    count++;
    Serial.println((String)count + "/200 sent");
  }
  listen_lora();
  if (count >= 200)
    while (1)
      ;
}