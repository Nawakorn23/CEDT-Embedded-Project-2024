#include <esp_now.h>
#include <WiFi.h>
#include "../lib/BH1750FVI/src/BH1750FVI.h"

// Wi-Fi Credentials
const char* ssid = "Donut";
const char* password = "11111111";

// กำหนดพินและตัวแปรสำหรับ Water Sensor
int waterSensorPin = 34; // พินที่ใช้สำหรับเซ็นเซอร์น้ำ
int waterVal = 0;

// กำหนดพินและตัวแปรสำหรับ Light Sensor
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
int ledPin = 23; // พินควบคุม LED
uint16_t lux = 0;

// MAC address ของอุปกรณ์ที่รับ (เปลี่ยนตามจริง)
uint8_t receiverMAC[] = {0x88, 0x13, 0xBF, 0x0D, 0x8F, 0x3C}; // ตัวอย่าง

// โครงสร้างข้อมูลที่ส่งไปยังตัวรับ
struct SensorData
{
  int waterLevel;
  uint16_t lightIntensity;
};

// Callback เมื่อส่งข้อมูลเสร็จ
void onSend(const uint8_t *macAddr, esp_now_send_status_t status)
{
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup()
{
  // การตั้งค่าสำหรับ Water Sensor และ Light Sensor
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  LightSensor.begin();

  // ตั้งค่า Wi-Fi โหมด Station
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set the same WiFi channel for ESP-NOW communication
  // uint8_t channel = 1; // Replace with the desired channel (1-11)
  // esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // เริ่มต้น ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }

  // เพิ่มข้อมูล peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMAC, 6); // เพิ่ม MAC address ของตัวรับ
  peerInfo.channel = 0;                       // ใช้ channel เดิม
  peerInfo.encrypt = false;                   // ไม่เข้ารหัส

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Peer added successfully");
}

void loop()
{
  // อ่านค่าจาก Water Sensor
  waterVal = analogRead(waterSensorPin);
  Serial.print("Water Sensor Value: ");
  Serial.println(waterVal);

  // อ่านค่าจาก Light Sensor
  lux = LightSensor.GetLightIntensity();
  Serial.print("Light Sensor Value: ");
  Serial.print(lux);
  Serial.println(" lux");

  // ควบคุม LED ตามค่าความสว่าง
  if (lux < 400)
  {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON");
  }
  else
  {
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF");
  }

  // สร้างโครงสร้างข้อมูลสำหรับส่ง
  SensorData data;
  data.waterLevel = waterVal;
  data.lightIntensity = lux;

  // ส่งข้อมูลไปยังตัวรับ
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));
  if (result == ESP_OK)
  {
    Serial.println("Data sent successfully");
  }
  else
  {
    Serial.println("Error sending data");
  }

  delay(2000); // ส่งข้อมูลทุก 2 วินาที
}
