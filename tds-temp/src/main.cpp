#include <Arduino.h>
#include <math.h>

// Thermistor Constants
#define RT0 10000       // Ω
#define B 3977          // K
#define VCC 3.3         // Supply voltage
#define R 10000         // Fixed resistor value (10KΩ)
#define SENSOR_PIN 34   // Thermistor ADC pin

// TDS Sensor Constants
#define TdsSensorPin 33 // TDS ADC pin
#define VREF 3.3        // Analog reference voltage (ESP32 = 3.3V)
#define SCOUNT 10       // Number of samples for TDS

// Thermistor Variables
float RT, VR, ln, Temp, T0_temp, Read;

// TDS Sensor Variables
int analogBuffer[SCOUNT];       // Buffer for TDS ADC readings
int analogBufferTemp[SCOUNT];   // Temporary buffer for sorting
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0;

// Timing Variable
unsigned long lastMeasurementTime = 0;

// Function to calculate median
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

void setup()
{
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(TdsSensorPin, INPUT);
  T0_temp = 25 + 273.15; // Reference temperature in Kelvin
}

void loop()
{
  unsigned long currentTime = millis();

  // Measure temperature and TDS every 1 second
  if (currentTime - lastMeasurementTime >= 1000)
  {
    lastMeasurementTime = currentTime;

    // 1. Read Temperature from Thermistor
    Read = analogRead(SENSOR_PIN); // Read ADC value
    Read = (VCC / 4095.0) * Read;  // Convert ADC to voltage
    VR = VCC - Read;
    RT = Read / (VR / R);          // Calculate resistance

    ln = log(RT / RT0);
    Temp = (1 / ((ln / B) + (1 / T0_temp))); // Temperature in Kelvin
    Temp = Temp - 273.15;                   // Convert to Celsius

    // 2. Read and Process TDS Sensor Data
    for (int i = 0; i < SCOUNT; i++)
    {
      analogBuffer[i] = analogRead(TdsSensorPin);
    }

    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4095.0;
    float compensationCoefficient = 1.0 + 0.02 * (Temp - 25.0); // Temperature compensation
    float compensationVoltage = averageVoltage / compensationCoefficient;
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage
                - 255.86 * compensationVoltage * compensationVoltage
                + 857.39 * compensationVoltage) * 0.5;

    // Print Temperature and TDS Value
    Serial.print("Temperature: ");
    Serial.print(Temp, 2); // Celsius
    Serial.print(" C\t");

    Serial.print("TDS Value: ");
    Serial.print(tdsValue, 0); // TDS in ppm
    Serial.println(" ppm");
  }
}
