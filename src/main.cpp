#include <Arduino.h>
#include <SPI.h>

#include <TFT_eSPI.h>
#include "esp_wifi.h"
#include <WiFi.h>
#include <WebServer.h>

/*
Felix Love
22153
NSN: 145498404

What does this program do?

What does the electronics do?

What is the project meant to do and how?

*/

TFT_eSPI tft = TFT_eSPI();

// DEBUG MODE - set to true to enable debug output, false to disable
bool DEBUG = true;

// wifi credentials
const char* ssid = "Code-ESP32";
const char* password = "12345678";

// create a web server object on port 80
//WebServer server(80);
//void handleRoot() {
//  server.send(200, "text/html", "<h1>ESP32 Online</h1>");
//}


// pin declaration variables


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

/* data processing funcs */

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

/* setup and loop functions */

void setup() {
// initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  while(!Serial) {
  }

  // initialize the TFT display
  tft.init();
  tft.setRotation(1); // try 0–3 if wrong
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Booting...");
  
  // create WiFi access point
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true, true);
  delay(1500);

  WiFi.mode(WIFI_AP);
  delay(1000);

  // IMPORTANT: force RF calibration re-init
  esp_wifi_stop();
  delay(200);
  esp_wifi_start();
  delay(500);

  bool ok = WiFi.softAP(ssid, password, 6, 0, 4);

  delay(500);

  Serial.println(ok ? "[INFO] AP STARTED" : "[ERROR] AP FAILED");
  Serial.print("[INFO] AP IP: ");
  Serial.println(WiFi.softAPIP());

  //server.on("/", handleRoot);
  //server.begin();

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.println("ESP32 AP Ready");
  tft.println("");
  tft.print("IP: ");
  //tft.println(ip);
  
  if (DEBUG)
  {
    debugOutputSTR("WiFi Access Point created with SSID: " + String(ssid));
  }
}

void loop() {
  //server.handleClient();
  delay(1000);
  Serial.println("Looping...");
}