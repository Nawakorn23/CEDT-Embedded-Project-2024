#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

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

// Function to calculate temperature from the thermistor
float getTemperature() {
    int rawADC = analogRead(thermistorPin); // Read raw ADC value
    float voltage = rawADC * (3.3 / 4095.0); // Convert ADC value to voltage
    float resistance = SERIES_RESISTOR / ((3.3 / voltage) - 1); // Calculate resistance
    float temperatureKelvin = 1 / ((log(resistance / R0) / BETA) + (1 / Kelvin25)); // Calculate temperature in Kelvin
    return temperatureKelvin - 273.15; // Convert Kelvin to Celsius
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

void setup() {
    Serial.begin(115200);

    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting To WiFi Network .");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Serve the HTML page
    server.on("/", HTTP_GET, handleRoot);

    // Serve temperature readings
    server.on("/temperature", HTTP_GET, handleTemperature);

    // Start the server
    server.begin();
}

void loop() {
    // Nothing needed here; AsyncWebServer handles requests
}
