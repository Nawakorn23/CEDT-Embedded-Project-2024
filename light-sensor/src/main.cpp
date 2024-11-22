#include "../lib/BH1750FVI/src/BH1750FVI.h" // ระบุเส้นทางที่ถูกต้อง
#include <Arduino.h>

BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
int ledPin = 23;
void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  LightSensor.begin();
}
void loop()
{
  uint16_t lux = LightSensor.GetLightIntensity();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");
  if (lux < 400)
  {                             // สามารถกำหนดค่าความสว่างตามต้องการได้
    digitalWrite(ledPin, HIGH); // สั่งให้ LED ติดสว่าง
    Serial.println("LED ON");
    Serial.println();
  }
  if (lux > 400)
  {                            // สามารถกำหนดค่าความสว่างตามต้องการได้
    digitalWrite(ledPin, LOW); // สั่งให้ LED ดับ
    Serial.println("LED OFF");
    Serial.println();
  }
  delay(1000);
}


// #include "../index-light.html"
// #include <Arduino.h>
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <SPIFFS.h>
// #include "../lib/BH1750FVI/src/BH1750FVI.h" // ระบุเส้นทางที่ถูกต้อง

// // ข้อมูลการเชื่อมต่อ Wi-Fi
// const char* ssid = "Nawa";
// const char* password = "12345678";

// // กำหนด GPIO สำหรับ LED
// int ledPin = 23;

// // ตั้งค่าการเชื่อมต่อ BH1750FVI
// BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

// // สร้าง WebSocket Server
// AsyncWebServer server(80);
// AsyncWebSocket ws("/ws");

// // ฟังก์ชันสำหรับการส่งข้อมูลเซ็นเซอร์ผ่าน WebSocket
// void notifyClients() {
//   uint16_t lux = LightSensor.GetLightIntensity();

//   // สร้างข้อมูลในรูปแบบ JSON
//   String json = "{\"lux\":";
//   json += String(lux);
//   json += "}";

//   // Debug
//   Serial.println("Light intensity: " + String(lux) + " lux");
//   Serial.println(json);

//   // ส่งข้อมูลไปยัง client
//   ws.textAll(json);

//   // ควบคุม LED ตามค่าความสว่าง
//   if (lux < 400) {
//     digitalWrite(ledPin, HIGH);
//     Serial.println("LED ON");
//   } else {
//     digitalWrite(ledPin, LOW);
//     Serial.println("LED OFF");
//   }
// }

// // ฟังก์ชัน callback เมื่อมีการเชื่อมต่อ WebSocket
// void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
//   if (type == WS_EVT_CONNECT) {
//     Serial.println("WebSocket client connected");
//     client->ping();
//   } else if (type == WS_EVT_DISCONNECT) {
//     Serial.println("WebSocket client disconnected");
//   }
// }

// void setup() {
//   // เริ่มต้น Serial Monitor
//   Serial.begin(115200);

//   // เริ่มต้น LED
//   pinMode(ledPin, OUTPUT);

//   // เริ่มต้นเซ็นเซอร์ BH1750FVI
//   LightSensor.begin();

//   // เริ่มต้น SPIFFS
//   if (!SPIFFS.begin()) {
//     Serial.println("An error occurred while mounting SPIFFS");
//     return;
//   }

//   // เชื่อมต่อ Wi-Fi
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }
//   Serial.println("Connected to WiFi");
//   Serial.print("Local ESP32 IP: ");
//   Serial.println(WiFi.localIP());

//   // เริ่มต้น WebSocket
//   ws.onEvent(onEvent);
//   server.addHandler(&ws);

//   // เสิร์ฟไฟล์ HTML
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
//     request->send(SPIFFS, "/index-light.html", "text/html");
//   });

//   // เริ่มต้นเซิร์ฟเวอร์
//   server.begin();
// }

// void loop() {
//   // ตรวจสอบข้อมูลเซ็นเซอร์และส่งข้อมูลทุกๆ 2 วินาที
//   static unsigned long lastTime = 0;
//   if (millis() - lastTime > 2000) {
//     notifyClients();
//     lastTime = millis();
//   }

//   ws.cleanupClients();
// }
