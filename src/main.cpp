#include <Arduino.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include "esp_wifi.h"
#include <WiFi.h>
#include <WebServer.h>

/*
Felix Love
22153
NSN: 145498404

What does this program do?
- Creates a Wi-Fi access point.
- Hosts a web page that displays and updates a temperature value.
- Displays status information on the built-in TFT screen.
*/

// DEBUG MODE - set to true to enable debug output, false to disable
bool DEBUG = true;

unsigned long lastPrint = 0;

// Wi-Fi credentials
const char* ssid = "Code-ESP32";
const char* password = "12345678";

// Built-in TFT display object (ST7789 controller)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Global variables
float tempValue = 1.1; // Example temperature value

/* -------------------------------------------------------------------------- */
/* Debug / Info Functions                                                      */
/* -------------------------------------------------------------------------- */

void debugOutputSTR(String data)
{
  if (!DEBUG) return;
  Serial.print("[DEBUG] ");
  Serial.println(data);
}

void debugOutputINT(String sensor, int data)
{
  if (!DEBUG) return;
  Serial.print("[DEBUG] ");
  Serial.print(sensor);
  Serial.print(": ");
  Serial.println(data);
}

int readSensorData(String sensor, byte pin)
{
  int sensorValue = analogRead(pin);
  debugOutputINT(sensor, sensorValue);
  return sensorValue;
}

void infoOutput(String str)
{
  Serial.print("[INFO] ");
  Serial.println(str);
}

/* -------------------------------------------------------------------------- */
/* Data Processing                                                             */
/* -------------------------------------------------------------------------- */

String processData(int sensorValue, String sensor)
{
  String processedData = "";

  if (sensor == "Temp")
  {
    processedData = String(sensorValue) + " °C";
  }

  debugOutputSTR("Processed data: " + processedData);
  return processedData;
}

/* -------------------------------------------------------------------------- */
/* Web Server                                                                  */
/* -------------------------------------------------------------------------- */

WebServer server(80);

void handleRoot()
{
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
      document.getElementById('temp').innerText = data.temp + ' °C';
    });
}

function sendTemp() {
  let val = document.getElementById('inputTemp').value;
  fetch('/set?temp=' + val);
}

setInterval(updateData, 2000);
updateData();
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

void handleData()
{
  String json = "{\"temp\":" + String(tempValue) + "}";
  server.send(200, "application/json", json);
}

void handleSet()
{
  if (server.hasArg("temp"))
  {
    tempValue = server.arg("temp").toFloat();
    debugOutputSTR("Temp updated from web: " + String(tempValue));

    // Update the TFT display with the new temperature
    tft.fillRect(0, 80, tft.width(), 30, ST77XX_BLACK);
    tft.setCursor(10, 80);
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(2);
    tft.print(tempValue);
    tft.print(" C");
  }

  server.send(200, "text/plain", "OK");
}

/* -------------------------------------------------------------------------- */
/* TFT Display Initialization                                                  */
/* -------------------------------------------------------------------------- */

void initDisplay()
{
  Serial.println("[INFO] ========== TFT DISPLAY SETUP ==========");

  // Enable power to the TFT and STEMMA/Qwiic circuitry
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(100);

  // Enable the backlight
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // Initialize the ST7789 display (135x240 pixels)
  tft.init(135, 240);
  tft.setRotation(1); // Landscape orientation

  Serial.printf("[INFO] TFT width=%d height=%d\n", tft.width(), tft.height());

  // Quick color test
  tft.fillScreen(ST77XX_RED);
  delay(500);
  tft.fillScreen(ST77XX_GREEN);
  delay(500);
  tft.fillScreen(ST77XX_BLUE);
  delay(500);
  tft.fillScreen(ST77XX_BLACK);

  // Initial text
  tft.setTextWrap(false);
  tft.setCursor(10, 20);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("SUCCESS!");

  tft.setTextSize(1);
  tft.setCursor(10, 50);
  tft.println("SPI communication works");

  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.setTextColor(ST77XX_CYAN);
  tft.print(tempValue);
  tft.print(" C");

  Serial.println("[INFO] ========== TFT READY ==========");
}

/* -------------------------------------------------------------------------- */
/* Setup                                                                       */
/* -------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}
  delay(1000);

  // Initialize the TFT display
  initDisplay();

  // Wi-Fi Access Point setup
  Serial.println("[INFO] ========== WIFI SETUP START ==========");

  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true, true);
  delay(1500);

  WiFi.mode(WIFI_AP);
  delay(1000);

  bool ok = WiFi.softAP(ssid, password, 6, 0, 4);

  Serial.println(ok ? "[INFO] AP STARTED" : "[ERROR] AP FAILED");
  Serial.print("[INFO] AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Update display with AP IP address
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(10, 120);
  tft.print("AP IP: ");
  tft.println(WiFi.softAPIP());

  // Web server routes
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/set", handleSet);
  server.begin();

  debugOutputSTR("WiFi Access Point created with SSID: " + String(ssid));
}

/* -------------------------------------------------------------------------- */
/* Main Loop                                                                   */
/* -------------------------------------------------------------------------- */

void loop()
{
  server.handleClient();

  if (millis() - lastPrint > 1000)
  {
    Serial.println("Looping...");
    lastPrint = millis();
  }
}