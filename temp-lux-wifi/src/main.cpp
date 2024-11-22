#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "../lib/BH1750FVI/src/BH1750FVI.h" // ระบุเส้นทางที่ถูกต้อง
#include <Arduino.h>
#include <math.h>
#include <Wire.h>

// Replace with your WiFi credentials
const char* ssid = "Donut";
const char* password = "11111111";

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

// Thermistor Pin and Constants
const int thermistorPin = 34;   // Analog pin connected to the thermistor voltage divider
const float BETA = 3950;        // Beta value of the thermistor
const float R0 = 10000;         // Resistance at 25 degrees Celsius
const float Kelvin25 = 298.15;  // Temperature at 25 degrees Celsius in Kelvin
const int SERIES_RESISTOR = 10000; // Resistor in series with the thermistor

BH1750FVI lightMeter(BH1750FVI::k_DevModeContLowRes);
const int ledPin = 23;

// Function to calculate temperature from the thermistor
float getTemperature() {
    int rawADC = analogRead(thermistorPin); // Read raw ADC value
    float voltage = rawADC * (3.3 / 4095.0); // Convert ADC value to voltage
    float resistance = SERIES_RESISTOR / ((3.3 / voltage) - 1); // Calculate resistance
    float temperatureKelvin = 1 / ((log(resistance / R0) / BETA) + (1 / Kelvin25)); // Calculate temperature in Kelvin
    return temperatureKelvin - 273.15; // Convert Kelvin to Celsius
}

// Function to get light intensity
uint16_t getLightIntensity() {
    uint16_t lux = lightMeter.GetLightIntensity(); // Correct method for BH1750
    return lux;
}

// Handle the root webpage
void handleRoot(AsyncWebServerRequest *request) {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      Serial.println("cannot open");
        request->send(500, "text/plain", "Cannot open file");
        return;
    }
    request->send(SPIFFS, "/index.html", String(), false);
    file.close();
}

// Handle temperature endpoint
void handleTemperature(AsyncWebServerRequest *request) {
    float temperature = getTemperature(); // Get current temperature
    request->send(200, "text/plain", String(temperature, 2)); // Send temperature as response
}

// Handle light intensity endpoint
void handleLightIntensity(AsyncWebServerRequest *request) {
    uint16_t lux = getLightIntensity(); // Get current light intensity
    request->send(200, "text/plain", String(lux));
}

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    lightMeter.begin();
    Wire.begin();

    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }

    // Initialize the sensor
//     if (!lightMeter.begin()) {
//     Serial.println("BH1750FVI initialization failed!");
//     while (1); // Halt execution
// }
//     Serial.println("BH1750FVI initialized.");

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting To WiFi Network");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Serve the HTML page
    server.on("/", HTTP_GET, handleRoot);

    // Serve temperature readings
    server.on("/temperature", HTTP_GET, handleTemperature);

    // Serve light intensity readings
    server.on("/light", HTTP_GET, handleLightIntensity);

    // Start the server
    server.begin();
}

void loop() {
    // Nothing needed here; AsyncWebServer handles requests
    uint16_t lux = lightMeter.GetLightIntensity();
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
