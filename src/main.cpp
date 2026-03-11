#include <Arduino.h>

void setup() {
  // initialize serial communication at 115200 bits per second:
Serial.begin(115200);
}

void loop() {
  Serial.println("Hello, world!");
}

/* 

Built in library: functions

*/
// read sensor data on provided pin and return the value
int readSensorData(byte pin)
{
    // reading sensor data
    int sensorValue = analogRead(pin);
    deubgOutputINT("Sensor", sensorValue);
    return sensorValue;
}

// print given debug information to the serial monitor | STRING
// debugOutputSTR("start of loop") will print: [DEBUG] start of loop
void deubgOutputSTR(String str)
{
    Serial.print("[DEBUG] ");
    Serial.println(str);
}

// print given debug information to the serial monitor | INT
// debugOutputINT("Temp", 25) will print: [DEBUG] Temp: 25
void deubgOutputINT(String sensor, int num)
{
    Serial.print("[DEBUG] ");
    Serial.print(sensor);
    Serial.print(": ");
    Serial.println(num);
}

// print given information to the serial monitor | STRING
// infoOutput("start of loop") will print: [INFO] start of loop
void infoOutput(String str)
{
    Serial.print("[INFO] ");
    Serial.println(str);
}