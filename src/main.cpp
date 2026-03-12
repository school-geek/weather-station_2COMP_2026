#include <Arduino.h>

bool DEBUG = true;

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
  debugOutputINT("Sensor", sensorValue);
  return sensorValue;
}

// print given information to the serial monitor | STRING
// infoOutput("start of loop") will print: [INFO] start of loop
void infoOutput(String str)
{
  Serial.print("[INFO] ");
  Serial.println(str);
}


//
// DEBUB funcs
//


// print given debug information to the serial monitor | STRING
// debugOutputSTR("start of loop") will print: [DEBUG] start of loop
void debugOutputSTR(String str)
{
  if (!DEBUG) return; // skip debug output if DEBUG is false
  Serial.print("[DEBUG] ");
  Serial.println(str);
}

// print given debug information to the serial monitor | INT
// debugOutputINT("Temp", 25) will print: [DEBUG] Temp: 25
void debugOutputINT(String sensor, int num)
{
  if (!DEBUG) return; // skip debug output if DEBUG is false
  Serial.print("[DEBUG] ");
  Serial.print(sensor);
  Serial.print(": ");
  Serial.println(num);
}