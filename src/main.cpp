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

// sensor libraries
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

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
bool DEBUG = false;

// VERBOSE DEBUGING MDOE - set to true for more detailed debug output, false for concise output
bool verbose = false;

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

Adafruit_BMP280 bmp;
Adafruit_AHTX0 aht;

/* -------------------------------------------------------------------------- */
/* Variables                                                                  */
/* -------------------------------------------------------------------------- */

/* Global variables */
float tempSensorValue = 0;

// Timing variable for debug output
unsigned long lastPrint = 0;

struct SensorData
{
  float bmpTemp;
  float bmpPressure;

  float ahtTemp;
  float ahtHumidity;

  // future sensors go here
  float extra1;
  float extra2;
};

/* GPIO pin definitions */
const int TEMPSENSOR = 5;

const int BUTTON_D0 = 0;
const int BUTTON_D1 = 1;
const int BUTTON_D2 = 2;

const int SDA_PIN = 3;
const int SCL_PIN = 4;

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
  if (DEBUG && verbose)
  {
    debugOutputINT(sensor, sensorValue);
  }
  return sensorValue;
}

int digiReadSensorData(String sensor, byte pin_var)
{
  int sensorValue = digitalRead(pin_var);
  if (DEBUG && verbose)
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

String processData(SensorData data)
{
  String out = "";

  // BMP280
  if (!isnan(data.bmpTemp) && !isnan(data.bmpPressure))
  {
    out += "BMP Temp: " + String(data.bmpTemp) + " °C";
    out += " | Pressure: " + String(data.bmpPressure) + " hPa";
  }
  else
  {
    out += "BMP: invalid";
  }

  out += " || ";

  // AHT20
  if (!isnan(data.ahtTemp) && !isnan(data.ahtHumidity))
  {
    out += "AHT Temp: " + String(data.ahtTemp) + " °C";
    out += " | Hum: " + String(data.ahtHumidity) + " %";
  }
  else
  {
    out += "AHT: invalid";
  }

  debugOutputSTR(out);
  return out;
}

/* -------------------------------------------------------------------------- */
/* Web Server & WebSocket Handling                                            */
/* -------------------------------------------------------------------------- */

// WebSocket Event Handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  if (type == WStype_TEXT)
  {
    String message = "";
    for (size_t i = 0; i < length; i++)
    {
      message += (char) payload[i];
    }
    
    if (DEBUG)
    {
      debugOutputSTR("WS received: " + message);
    }
    
    if (message.startsWith("command:"))
{
  String cmd = message.substring(8); // remove "command:"

  cmd.trim();

  // Split command + argument
  int commaIndex = cmd.indexOf(',');

  String action = "";
  String value = "";

  if (commaIndex == -1)
  {
    action = cmd;
  }
  else
  {
    action = cmd.substring(0, commaIndex);
    value  = cmd.substring(commaIndex + 1);
  }

  action.trim();
  value.trim();

  if (DEBUG)
  {
    debugOutputSTR("CMD: " + action + " | VAL: " + value);
  }

  // =========================
  // COMMAND ROUTING
  // =========================

  if (action == "update")
  {
    webSocket.sendTXT(num, "updated");
  }

  else if (action == "status")
  {
;
//    webSocket.sendTXT(num, statusMsg);
  }

  else
  {
    webSocket.sendTXT(num, "ERR: unknown command");
  }
}
  }
}

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

  <input type="text" id="inputTemp" placeholder="Send command...">
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
    ws.send("command:" + val);
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
  tft.print(tempSensorValue);
  tft.print(" C");

  infoOutput("-- TFT READY --");
}

/* -------------------------------------------------------------------------- */
/* Sensor Initialization                                                      */
/* -------------------------------------------------------------------------- */

void initBMP280()
{
  if (bmp.begin(0x76)) {
    infoOutput("BMP280 found at 0x76.");
  } else if (bmp.begin(0x77)) {
    infoOutput("BMP280 found at 0x77.");
  } else {
    infoOutput("BMP280 not found.");
  }
}

/* -------------------------------------------------------------------------- */
void initAHTX0()
{
  if (aht.begin()) {
    infoOutput("AHT20 found.");
  } else {
    infoOutput("[ERROR] AHT20 not found.");
  }
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

  // WebSocket setup
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  /* Pin initialization */
  pinMode(TEMPSENSOR, INPUT);

  pinMode(BUTTON_D0, INPUT_PULLUP);
  pinMode(BUTTON_D1, INPUT_PULLDOWN);
  pinMode(BUTTON_D2, INPUT_PULLDOWN);

  initBMP280();
  initAHTX0();

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

}

void updateSensors()
{
  SensorData data;

  // BMP280
  if (bmp.begin(0x76) || bmp.begin(0x77))
  {
  data.bmpTemp = bmp.readTemperature();
  data.bmpPressure = bmp.readPressure() / 100.0;
  }
  else
  {
    data.bmpTemp = NAN;
    data.bmpPressure = NAN;
  }

  // AHT20
  sensors_event_t humidity, temp;
  if (aht.getEvent(&humidity, &temp))
  {
    data.ahtTemp = temp.temperature;
    data.ahtHumidity = humidity.relative_humidity;
  }
  else
  {
    data.ahtTemp = NAN;
    data.ahtHumidity = NAN;
  }

  // Process everything
  String output = processData(data);
  sendToWebsite(output);
}

/* -------------------------------------------------------------------------- */
/* Main Loop                                                                  */
/* -------------------------------------------------------------------------- */

void loop()
{
  // web server and websocket handling
  server.handleClient();
  webSocket.loop();

   if (millis() - lastPrint > 5000)
   {
    lastPrint = millis();
    updateSensors();
   }
  
  byte D0_state = digiReadSensorData("D0", BUTTON_D0);
  byte D1_state = digiReadSensorData("D1", BUTTON_D1);
  byte D2_state = digiReadSensorData("D2", BUTTON_D2);

  String serialInput = Serial.readString();
  if(serialInput == "update")
  {
    infoOutput("Updating sensor data...");
    infoOutput("COMMENTED OUT");
    /*
    int tempSensorValue = anaReadSensorData("temp", TEMPSENSOR);
    String processedTemp = processData(tempSensorValue, "temp");
    sendToWebsite(processedTemp);

    // Update TFT display with new temperature value
    tft.fillRect(10, 80, 100, 30, ST77XX_BLACK); // Clear previous value
    tft.setCursor(10, 80);
    tft.setTextColor(ST77XX_CYAN);
    tft.print(tempSensorValue);
    tft.print(" C");
    */
  }
  else if (serialInput.startsWith("send "))
  {
    // Extract everything after "send "
    String dataToSend = serialInput.substring(5);

    // Send the extracted data to the website
    sendToWebsite(dataToSend);

    // Optional confirmation in Serial Monitor
    infoOutput("Sent to website: ");
    infoOutput(dataToSend);
  }

  if (D0_state == LOW)
  {
    infoOutput("Button D0 pressed!");
  }
  
  if (D1_state == HIGH)
  {
    infoOutput("Button D1 pressed!");
  }

  if (D2_state == HIGH)
  {
    infoOutput("Button D2 pressed!");
  }
}