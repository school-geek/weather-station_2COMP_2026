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

// DEBUG MODE - set to true to enable debug output, false to disable
bool DEBUG = true;

unsigned long lastPrint = 0;

// wifi credentials
const char* ssid = "Code-ESP32";
const char* password = "12345678";

TFT_eSPI tft = TFT_eSPI();

// pin declaration variables


// global variables
float tempValue = 1.1; // example temperature value, will be updated from sensor readings and web input

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

/* create a web server object on port 80 */
WebServer server(80);
void handleRoot() {
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Panel</title>
  <style>
    body {
      font-family: Arial;
      background: #121212;
      color: white;
      text-align: center;
    }
    .card {
      background: #1e1e1e;
      padding: 20px;
      margin: 20px auto;
      border-radius: 10px;
      width: 300px;
    }
    input, button {
      padding: 10px;
      margin-top: 10px;
      width: 80%;
      border-radius: 5px;
      border: none;
    }
    button {
      background: #00c3ff;
      color: black;
      font-weight: bold;
    }
  </style>
</head>
<body>

<h1>ESP32 Control Panel</h1>

<div class="card">
  <h2>Temperature</h2>
  <p id="temp">--</p>

  <input type="number" id="inputTemp" placeholder="Set temp">
  <button onclick="sendTemp()">Update</button>
</div>

<script>
function updateData() {
  fetch('/data')
    .then(r => r.json())
    .then(data => {
      document.getElementById("temp").innerText = data.temp + " °C";
    });
}

function sendTemp() {
  let val = document.getElementById("inputTemp").value;
  fetch('/set?temp=' + val);
}

// refresh every 2 seconds
setInterval(updateData, 2000);
updateData();
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}


void handleData() {
  String json = "{\"temp\":" + String(tempValue) + "}";
  server.send(200, "application/json", json);
}

void handleSet() {
  if (server.hasArg("temp")) {
    tempValue = server.arg("temp").toFloat();

    debugOutputSTR("Temp updated from web: " + String(tempValue));
  }
  server.send(200, "text/plain", "OK");
}


/* setup and loop functions */

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  while(!Serial) {
  }
  delay(1000);

  Serial.println("[INFO] ========== TFT_eSPI DIRECT TEST ==========");

  /* Try TFT_eSPI initialization directly - skip manual SPI testing */
  Serial.println("[INFO] Setting up TFT pins and power...");

  // Set up I2C/TFT power pin - REQUIRED for display to work
  if (TFT_I2C_POWER >= 0) {
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(100);
    Serial.println("[INFO] TFT_I2C_POWER pin set to HIGH (required for display)");
  }

  // Set up backlight - turn off first, then on
  if (TFT_BACKLITE >= 0) {
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, LOW);  // Turn off first
    delay(100);
    digitalWrite(TFT_BACKLITE, HIGH); // Then turn on
    Serial.println("[INFO] Backlight cycled OFF→ON");
  }

  Serial.println("[INFO] Calling tft.init()...");
  tft.init();
  Serial.println("[INFO] TFT init complete!");

  tft.setRotation(1);  // Try rotation 1
  Serial.printf("[INFO] TFT width=%d height=%d\n", tft.width(), tft.height());

  // Quick color test - simplified
  Serial.println("[INFO] Quick color test...");
  tft.fillScreen(TFT_RED);
  delay(2000);  // Longer delay
  tft.fillScreen(TFT_GREEN);
  delay(2000);
  tft.fillScreen(TFT_BLUE);
  delay(2000);
  tft.fillScreen(TFT_WHITE);
  delay(2000);
  tft.fillScreen(TFT_BLACK);
  Serial.println("[INFO] Color test complete - check if colors are visible");
  delay(500);
  tft.fillScreen(TFT_GREEN);
  delay(500);
  tft.fillScreen(TFT_BLUE);
  delay(500);
  tft.fillScreen(TFT_BLACK);
  delay(500);

  // Display text
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.println("SUCCESS!");
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  tft.println("SPI communication works");

  Serial.println("[INFO] ========== FULL TFT TEST COMPLETE ==========");

  // Display text
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.println("SUCCESS!");
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  tft.println("SPI communication works");

  Serial.println("[INFO] ========== FULL TFT TEST COMPLETE ==========");

  /* create WiFi access point */
  Serial.println("[INFO] ========== WIFI SETUP START ==========");
  Serial.println("[INFO] Disabling WiFi...");
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true, true);
  delay(1500);
  Serial.println("[INFO] WiFi disabled");

  WiFi.mode(WIFI_AP);
  delay(1000);
  Serial.println("[INFO] WiFi AP mode set");

  Serial.println("[INFO] Creating AP...");
  bool ok = WiFi.softAP(ssid, password, 6, 0, 4);
  delay(500);
  Serial.println(ok ? "[INFO] AP STARTED" : "[ERROR] AP FAILED");
  Serial.print("[INFO] AP IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("[INFO] ========== WIFI SETUP COMPLETE ==========");

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/set", handleSet);
  server.begin();

  if (DEBUG)
  {
    debugOutputSTR("WiFi Access Point created with SSID: " + String(ssid));
  }
}

void loop() {
  server.handleClient();

  if (millis() - lastPrint > 1000) {
    Serial.println("Looping...");
    lastPrint = millis();
  }
}