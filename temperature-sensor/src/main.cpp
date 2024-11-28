#include <Arduino.h>
#include <math.h>

#define RT0 10000     // Ω
#define B 3977        // K
#define VCC 3.3       // แรงดันไฟฟ้าเลี้ยง
#define R 10000       // ตัวต้านทานคงที่ R=10KΩ
#define SENSOR_PIN 34 // ขา ADC 36

// Variables
float RT, VR, ln, Temp, T0_temp, Read;

void setup()
{
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
  T0_temp = 25 + 273.15; // อุณหภูมิอ้างอิงในเคลวิน
}

void loop()
{
  Read = analogRead(SENSOR_PIN); // อ่านค่า ADC
  Read = (VCC / 4095.0) * Read;  // แปลงค่า ADC เป็นแรงดันไฟฟ้า
  VR = VCC - Read;
  RT = Read / (VR / R); // คำนวณความต้านทาน RT

  ln = log(RT / RT0);
  Temp = (1 / ((ln / B) + (1 / T0_temp))); // อุณหภูมิในเคลวิน

  Temp = Temp - 273.15; // แปลงเป็นองศาเซลเซียส

  // แสดงผลลัพธ์
  Serial.print("Temperature:");
  Serial.print("\t");
  Serial.print(Temp); // Celsius
  Serial.print("C\t\t");
  Serial.print(Temp + 273.15); // Kelvin
  Serial.print("K\t\t");
  Serial.print((Temp * 1.8) + 32); // Fahrenheit
  Serial.println("F");

  delay(1000); // รอ 1 วินาที
}
