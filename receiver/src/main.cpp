#include <WiFi.h>
#include <esp_now.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>



// Wi-Fi Credentials
const char* ssid = "Donut";
const char* password = "11111111";

// Firebase Config
#define API_KEY "AIzaSyBq9hl0wAQIoJE23duUVDbvMTUetpdAijA"
#define DATABASE_URL "https://lux-lv-default-rtdb.asia-southeast1.firebasedatabase.app/"
// Firebase Data Object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Async Web Server on Port 80
// AsyncWebServer server(80);

// โครงสร้างข้อมูลที่รับ
struct SensorData {
  int waterLevel;
  uint16_t lightIntensity;
};

SensorData receivedData;

// Callback เมื่อได้รับข้อมูล
void onReceive(const uint8_t *macAddr, const uint8_t *incomingData, int len) {
  SensorData receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData)); // ดึงข้อมูลจาก incomingData

  // แสดงค่าที่ได้รับใน Serial Monitor
  Serial.print("Received Water Level: ");
  Serial.println(receivedData.waterLevel);
  Serial.print("Received Light Intensity: ");
  Serial.println(receivedData.lightIntensity);

  if (Firebase.ready()) {
    Serial.print("Firebase ready");
    String path = "/sensor_data";
    FirebaseJson json;
    json.set("water_level", receivedData.waterLevel);
    json.set("light_intensity", receivedData.lightIntensity);

    WiFi.setHostname("ESP32_Device");
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE); 

    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
      Serial.println("Data uploaded to Firebase");
    } else {
      Serial.print("Firebase Error: ");
      Serial.println(fbdo.errorReason());
    }
  }
}


void setup() {
  Serial.begin(115200);

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


  // Firebase Initialization
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = "test1@testmail.com";
  auth.user.password = "123456";
  Firebase.begin(&config, &auth);

  // เริ่มต้น ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }

  // ลงทะเบียน callback เมื่อได้รับข้อมูล
  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver ready, hi ");

  // Configure Web Server Routes
  // server.on("/", HTTP_GET, handleRoot);
  // server.on("/sensor-data", HTTP_GET, handleSensorData);
  // server.begin();
  // Serial.println("Web Server started donut");
}

void loop() {
  // รอรับข้อมูล
}
