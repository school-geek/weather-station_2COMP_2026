#include <Arduino.h>
#include <SPI.h>

/*
Felix Love
22153
NSN: 0145498404

What does this program do?

What does the electronics do?

What is the project meant to do and how?

*/

// DEBUG MODE - set to true to enable debug output, false to disable
bool DEBUG = true;

// pin declaration variables


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

// basic funcs

// print given debug information to the serial monitor | STRING
// debugOutputSTR("start of loop") will print: [DEBUG] start of loop
void debugOutputSTR(String data)
{
  if (!DEBUG) return; // skip debug output if DEBUG is false
  Serial.print("[DEBUG] ");
  Serial.println(data);
}

// print given debug information to the serial monitor | INT
// debugOutputINT("Temp", 25) will print: [DEBUG] Temp: 25
void debugOutputINT(String sensor, int data)
{
  if (!DEBUG) return; // skip debug output if DEBUG is false
  Serial.print("[DEBUG] ");
  Serial.print(sensor);
  Serial.print(": ");
  Serial.println(data);
}

// read sensor data on provided pin and return the value
int readSensorData(String sensor, byte pin)
{
  // reading sensor data
  int sensorValue = analogRead(pin);
  if (DEBUG)
  {
    debugOutputINT(sensor, sensorValue);
  }
  return sensorValue;
}

// print given information to the serial monitor | STRING
// infoOutput("start of loop") will print: [INFO] start of loop
void infoOutput(String str)
{
  Serial.print("[INFO] ");
  Serial.println(str);
}

// data processing funcs

// process raw sensor data and return a formatted string
// processData(25, "Temp") will return "25 °C"
String processData(int sensorValue, String sensor)
{
  String processedData = ""; // initialize processedData as an empty string
 
  if (sensor == "Temp")
  {
    // sensorValue = map(sensorValue, 000, 000, 000, 000);
    processedData = String(sensorValue) + " °C";
  }
  /*
  else if (sensor == "Light")
  {
    // sensorValue = map(sensorValue, 000, 000, 000, 000);
    processedData = String(sensorValue) + " %";
  }
  */
  if (DEBUG)
  {
    debugOutputSTR("Processed data: " + processedData);
  }
  return processedData;
}

