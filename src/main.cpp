#include <Arduino.h>

// display libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// wifi and web server libraries
#include "esp_wifi.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

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

/* -------------------------------------------------------------------------- */
/* Library class definitions and Library related stuff                        */
/* -------------------------------------------------------------------------- */

// Wi-Fi credentials
const char* ssid = "Code-ESP32";
const char* password = "12345678";

// Built-in TFT display object (ST7789 controller)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// websocket server
WebSocketsServer webSocket = WebSocketsServer(81);

/* -------------------------------------------------------------------------- */
/* Variables                                                                  */
/* -------------------------------------------------------------------------- */

// Global variables
float tempValue = 1.1; // Example temperature value

// Timing variable for debug output
unsigned long lastPrint = 0;

/* GPIO pin definitions */
const int TEMPSENSOR = 5;

const int BUTTON_D0 = 0;
const int BUTTON_D1 = 1;
const int BUTTON_D2 = 2;

/* -------------------------------------------------------------------------- */
/* Debug / Info Functions                                                     */
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

int anaReadSensorData(String sensor, byte pin_var)
{
  int sensorValue = analogRead(pin_var);
  if (DEBUG)
  {
    debugOutputINT(sensor, sensorValue);
  }
  return sensorValue;
}

int digiReadSensorData(String sensor, byte pin_var)
{
  int sensorValue = digitalRead(pin_var);
  if (DEBUG)
  {
    debugOutputINT(sensor, sensorValue);
  }
  return sensorValue;
}


void infoOutput(String str)
{
  Serial.print("[INFO] ");
  Serial.println(str);
}

/* -------------------------------------------------------------------------- */
/* Data Processing                                                            */
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
/* Web Server & WebSocket Handling                                            */
/* -------------------------------------------------------------------------- */

// WebSocket data sending
void sendToWebsite(String data)
{
  if (DEBUG)
  {
    debugOutputSTR("WS send: " + data);
  }

  webSocket.broadcastTXT(data);
}

WebServer server(80);
void handleRoot()
{
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 WebSocket Panel</title>

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
      cursor: pointer;
    }

    #status {
      font-size: 12px;
      color: #aaa;
      margin-top: 10px;
    }
  </style>
</head>

<body>

<h1>ESP32 Control Panel</h1>

<div class="card">
  <h2>Live Data</h2>
  <p id="temp">--</p>

  <input type="number" id="inputTemp" placeholder="Send value">
  <button onclick="sendData()">Send</button>

  <div id="status">Connecting...</div>
</div>

<script>
let ws;

function connectWS()
{
  ws = new WebSocket("ws://" + location.hostname + ":81");

  ws.onopen = function()
  {
    document.getElementById("status").innerText = "Connected";
  };

  ws.onclose = function()
  {
    document.getElementById("status").innerText = "Disconnected (retrying...)";
    setTimeout(connectWS, 2000);
  };

  ws.onmessage = function(event)
  {
    document.getElementById("temp").innerText = event.data;
  };
}

function sendData()
{
  let val = document.getElementById("inputTemp").value;

  if (ws && ws.readyState === 1)
  {
    ws.send("set:" + val);
  }
}

connectWS();
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

/* -------------------------------------------------------------------------- */
/* TFT Display Initialization                                                 */
/* -------------------------------------------------------------------------- */

void initDisplay()
{
  debugOutputSTR("== TFT DISPLAY SETUP ==");

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
 
  if (DEBUG) {
    int width = tft.width();
    int height = tft.height();
    debugOutputSTR("TFT width=" + String(width) + " height=" + String(height));

    debugOutputSTR("Colour test starting...");
  }

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
  tft.println("Weather Station");

  tft.setTextSize(1);
  tft.setCursor(10, 50);

  tft.setTextSize(2);
  tft.setCursor(10, 80);
  tft.setTextColor(ST77XX_CYAN);
  tft.print(tempValue);
  tft.print(" C");

  infoOutput("-- TFT READY --");
}

/* -------------------------------------------------------------------------- */
/* Setup                                                                      */
/* -------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(115200);
  while (!Serial) {}
  delay(1000);

  /* Initialize the TFT display */
  initDisplay();

  /* Wi-Fi Access Point setup */
  infoOutput("== WIFI SETUP START ==");

  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true, true);
  delay(1500);

  WiFi.mode(WIFI_AP);
  delay(1000);

  bool ok = WiFi.softAP(ssid, password, 6, 0, 4);

  if (DEBUG && ok) {
    debugOutputSTR("AP STARTED");

    debugOutputSTR("AP SSID: " + String(ssid));
    debugOutputSTR("AP Password: " + String(password));

    debugOutputSTR("AP IP: " + WiFi.softAPIP().toString());
  }
  else if (!ok)
  {
    debugOutputSTR("[ERROR] Failed to start AP");
  }
  

  // Update display with AP IP address
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(10, 120);
  tft.print("AP IP: ");
  tft.println(WiFi.softAPIP());

  // Web server routes
  server.on("/", handleRoot);
  server.begin();

  /* Pin initialization */
  pinMode(TEMPSENSOR, INPUT);

  pinMode(BUTTON_D0, INPUT_PULLUP);
  pinMode(BUTTON_D1, INPUT_PULLDOWN);
  pinMode(BUTTON_D2, INPUT_PULLDOWN);
}

/* -------------------------------------------------------------------------- */
/* Main Loop                                                                  */
/* -------------------------------------------------------------------------- */

void loop()
{
  // web server and websocket handling
  server.handleClient();
  webSocket.loop();

  if (DEBUG)
  {
   if (millis() - lastPrint > 1000)
   {
    Serial.println("Looping...");
    lastPrint = millis();
   }
  }

  
  byte D0_state = digiReadSensorData("D0", BUTTON_D0);
  byte D1_state = digiReadSensorData("D1", BUTTON_D1);
  byte D2_state = digiReadSensorData("D2", BUTTON_D2);
}