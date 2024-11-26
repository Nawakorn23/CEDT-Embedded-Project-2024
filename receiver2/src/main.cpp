#define BLYNK_TEMPLATE_ID "TMPL6KGtHgx8M"
#define BLYNK_TEMPLATE_NAME "luxlvl"
#define BLYNK_AUTH_TOKEN "1lKex0q-RPGf0IED_l-tBtd62KX7VnNO"

#include <WiFi.h>
#include <esp_now.h>
#include <BlynkSimpleEsp32.h>

// Wi-Fi Credentials
// const char* ssid = "S22";
// const char* password = "nawakorn";
  const char* ssid = "Donut";
  const char* password = "11111111";

// Blynk Auth Token
char auth[] = "1lKex0q-RPGf0IED_l-tBtd62KX7VnNO";  // Replace with your Blynk Auth Token

// Structure to hold sensor data
struct SensorData {
  int waterLevel;
  uint16_t lightIntensity;
};

SensorData receivedData;

// ESP-NOW Callback Function
void onReceive(const uint8_t* macAddr, const uint8_t* incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData)); // Extract sensor data
  Serial.printf("Received Water Level: %d, Light Intensity: %d\n", receivedData.waterLevel, receivedData.lightIntensity);

  // Send data to Blynk
  Blynk.virtualWrite(V0, receivedData.waterLevel);       // Send Water Level to Virtual Pin V1
  Blynk.virtualWrite(V1, receivedData.lightIntensity);   // Send Light Intensity to Virtual Pin V2
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());

  // Initialize Blynk
  Blynk.begin(auth, ssid, password);
  Serial.println("Blynk initialized");

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed!");
    return;
  }
  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver ready");
}

void loop() {
  Blynk.run();  // Run Blynk
}
