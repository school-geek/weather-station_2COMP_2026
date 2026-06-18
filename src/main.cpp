// the main guy. Its him
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
// temperature and pressure sensor
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

// light sensor
#include <Adafruit_LTR390.h>

/*
Felix Love
22153
NSN: 145498404

What does this program do?
- Creates a Wi-Fi access point.
- Hosts a web page that displays and updates a temperature value.
- Displays status information on the built-in TFT screen.
- Reads data from connected sensors (BMP280, AHT20, LTR390) and processes it into a weather forecast sentence.
- Sends the weather forecast to connected clients via WebSockets.
- Displays it to the TFT screen as well.
- The weather forecast is generated based on the sensor data and includes sky conditions, 
temperature description, humidity description, and precipitation likelihood.
- Includes debug output that can be enabled or disabled with the DEBUG and VERBOSE flags.

How does it work?
1. The program initializes the TFT display and sets up the Wi-Fi access point.
2. It initializes the sensors and checks if they are present.
3. In the main loop, it reads data from the sensors, processes it into a weather forecast, and sends it to connected clients.
*/

// DEBUG MODE - set to true to enable debug output, false to disable
bool DEBUG = false;

// VERBOSE DEBUGING MDOE - set to true for more detailed debug output, false for concise output
bool VERBOSE = false;

/* -------------------------------------------------------------------------- */
/* Library class definitions and Library related stuff                        */
/* -------------------------------------------------------------------------- */

// Wi-Fi credentials
const char* SSID = "Code-ESP32";
const char* PASSWORD = "12345678";

// Built-in TFT display object (ST7789 controller)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// websocket server
WebSocketsServer webSocket = WebSocketsServer(81);

/* sensor objects */
// BMP280 for temperature and pressure
Adafruit_BMP280 bmp;

// AHT20 for temperature and humidity
Adafruit_AHTX0 aht;

// LTR390 for light and UV
Adafruit_LTR390 ltr;

/* -------------------------------------------------------------------------- */
/* Variables                                                                  */
/* -------------------------------------------------------------------------- */

/* Global variables */
float tempSensorValue = 0;

// Timing variable for debug output
unsigned long lastPrint = 0;

// last pressure value
float lastPressure = NAN;

// presence flags to avoid re-initializing sensors repeatedly
bool bmpPresent = false;

const int sensorUpdateInterval = 10000; // update every 10 seconds

const int newDataInterval = 1500; // new data every 1.5 seconds

const int formatMessageOptions[] = {10, 30, 25, 20, 3}; // options for the displayFormattedMessage function: x=10, y=30, lineHeight=25, maxCharsPerLine=20, maxWordsPerLine=3

// page state
enum DisplayPage
{
  PAGE_HOME,
  PAGE_FORECAST,
  PAGE_DATA
};

// set page to home by default
DisplayPage currentPage = PAGE_HOME;

// struct to hold all sensor data in one place for easy passing around
struct SensorData
{
  float bmpTemp;
  float bmpPressure;

  float ahtTemp;
  float ahtHumidity;

  float ltrALS;
  float ltrUVS;
  float ltrUVIndex;
  float ltrLux;

  // future sensors go here
  float extra1;
  float extra2;
};

// weather model struct to hold processed weather information for the "metservice" style forecast generation
struct WeatherModel
{
  float temp;
  float humidity;
  float light;
  float pressure;
  float pressureTrend;
};

/* Weather words */
// Sky conditions
const char* skyOptions[] = {
  "Clear skies",
  "Sunny",
  "Mostly clear",
  "Partly cloudy",
  "Cloudy",
  "Overcast"
};

// Temperature phrases
const char* tempOptions[] = {
  "very cold",
  "cold",
  "cool",
  "mild",
  "warm",
  "hot",
  "very hot"
};

// Humidity phrases
const char* humidityOptions[] = {
  "and very dry",
  "and dry",
  "and comfortable",
  "and humid",
  "and very humid"
};

// Precipitation phrases
const char* precipOptions[] = {
  "with no precipitation expected",
  "and dry conditions",
  "with a few showers possible",
  "with isolated showers",
  "with occasional showers",
  "with periods of rain"
};

// Pressure trend
const char* pressureOptions[] = {
  "Conditions are stable.",
  "Conditions are improving.",
  "Conditions are becoming unsettled.",
  "Conditions are deteriorating."
};

/* GPIO pin definitions */
const int BUTTON_D0 = 0;
const int BUTTON_D1 = 1;
const int BUTTON_D2 = 2;

const int SDA_PIN = 3;
const int SCL_PIN = 4;

/* -------------------------------------------------------------------------- */
/* Debug / Info Functions                                                     */
/* -------------------------------------------------------------------------- */

// Debug output function for string messages, with a [DEBUG] prefix for clarity
void debugOutputSTR(String data)
{
  if (!DEBUG) return;
  Serial.print("[DEBUG] ");
  Serial.println(data);
}

// Debug output function for integer values, with a sensor name for context
void debugOutputINT(String sensor, int data)
{
  if (!DEBUG) return;
  Serial.print("[DEBUG] ");
  Serial.print(sensor);
  Serial.print(": ");
  Serial.println(data);
}

// Helper function to categorize UV Index
String getUVCategory(float uvIndex)
{
  if (uvIndex < 3) {
    return "Low";
  } else if (uvIndex < 6) {
    return "Moderate";
  } else if (uvIndex < 8) {
    return "High";
  } else if (uvIndex < 11) {
    return "Very High";
  } else {
    return "Extreme";
  }
}

// Reads analog sensor data from a specified pin and optionally outputs debug information.
int anaReadSensorData(String sensor, byte pin_var)
{
  int sensorValue = analogRead(pin_var);
  if (DEBUG && VERBOSE)
  {
    debugOutputINT(sensor, sensorValue);
  }
  return sensorValue;
}

// digital read version of the above function, for sensors that output digital signals instead of analog voltages
int digiReadSensorData(String sensor, byte pin_var)
{
  int sensorValue = digitalRead(pin_var);
  if (DEBUG && VERBOSE)
  {
    debugOutputINT(sensor, sensorValue);
  }
  return sensorValue;
}

// simple info output function for important messages that should always be printed, even if debug mode is off
void infoOutput(String str)
{
  Serial.print("[INFO] ");
  Serial.println(str);
}

// Helper function to convert LTR390 gain enum to actual gain factor for calculations
float getLTRGainFactor(ltr390_gain_t gain)
{
  switch (gain)
  {
    case LTR390_GAIN_1: return 1.0;
    case LTR390_GAIN_3: return 3.0;
    case LTR390_GAIN_6: return 6.0;
    case LTR390_GAIN_9: return 9.0;
    case LTR390_GAIN_18: return 18.0;
    default: return 1.0;
  }
}

// Helper function to convert LTR390 resolution enum to actual bit depth for calculations
int getLTRResolutionBits(ltr390_resolution_t res)
{
  switch (res)
  {
    case LTR390_RESOLUTION_20BIT: return 20;
    case LTR390_RESOLUTION_19BIT: return 19;
    case LTR390_RESOLUTION_18BIT: return 18;
    case LTR390_RESOLUTION_17BIT: return 17;
    case LTR390_RESOLUTION_16BIT: return 16;
    case LTR390_RESOLUTION_13BIT: return 13;
    default: return 16;
  }
}

// Converts raw LTR390 UV sensor data to a UV index value, taking into account the current gain and resolution settings of the sensor.
float computeLTRUVIndex(uint32_t raw, ltr390_gain_t gain, ltr390_resolution_t res)
{
  // The raw UV count from the LTR390 already reflects the current gain.
  // Use a simple conversion constant so higher gain raises the reported index.
  const float UV_INDEX_SCALE = 23.0; // empirical calibration constant for your current setup
  return raw / UV_INDEX_SCALE;
}

/* Weather condition helpers */
// Sky condition based on light level (lux)
const char* getSky(float lux)
{
  if (lux < 50) return "Overcast";
  if (lux < 500) return "Cloudy";
  if (lux < 3000) return "Partly cloudy";
  return "Sunny";
}

// Temperature description based on Celsius value
const char* getTemp(float t)
{
  if (t < 5) return "very cold";
  if (t < 12) return "cold";
  if (t < 18) return "cool";
  if (t < 24) return "mild";
  if (t < 30) return "warm";
  if (t < 36) return "hot";
  return "very hot";
}

// Humidity description based on percentage
const char* getHumidity(float h)
{
  if (h < 20) return "and very dry";
  if (h < 40) return "and dry";
  if (h < 60) return "and comfortable";
  if (h < 80) return "and humid";
  return "and very humid";
}

// Precipitation likelihood based on humidity and pressure trend
const char* getPrecip(float h, float trend)
{
  if (trend < -2) return "with occasional showers";
  if (trend < -1) return "with a few showers possible";
  if (h < 40) return "with no precipitation expected";
  return "and dry conditions";
}

// Helper function to clamp an index within array bounds
int clampIndex(int i, int maxSize) {
  if (i < 0) return 0;
  if (i >= maxSize) return maxSize - 1;
  return i;
}

// Builds a simple weather model struct from the raw sensor data, including a pressure trend calculation.
WeatherModel buildWeatherModel(const SensorData &data)
{
  WeatherModel w;

  w.temp = data.ahtTemp;
  w.humidity = data.ahtHumidity;
  w.light = data.ltrALS;
  w.pressure = data.bmpPressure;

  if (!isnan(lastPressure) && !isnan(w.pressure))
  {
    w.pressureTrend = w.pressure - lastPressure;
  }
  else
  {
    w.pressureTrend = 0;
  }

  lastPressure = w.pressure;

  return w;
}

String buildTFTForecastMessage(const SensorData &data, const WeatherModel &w)
{
  const float lux = 0.6f * data.ltrALS / (3.0f * 1.0f);
  const float uvIndex = computeLTRUVIndex(data.ltrUVS, ltr.getGain(), ltr.getResolution());

  const char* sky = getSky(lux);
  const char* temp = getTemp(w.temp);
  const char* hum = getHumidity(w.humidity);
  const char* precip = getPrecip(w.humidity, w.pressureTrend);

  const int styleIndex = clampIndex(random(0, 4), 3);
  const String skyText = String(sky);
  const String tempText = String(temp);
  const String humText = String(hum);
  const String precipText = String(precip);
  const String uvCategory = getUVCategory(uvIndex);

  if (styleIndex == 0)
  {
    return skyText + ", " + tempText + " " + humText + " " + precipText;
  }
  else if (styleIndex == 1)
  {
    return skyText + " with " + tempText + " conditions " + humText;
  }
  else
  {
    return skyText + " (" + uvCategory + ") with " + tempText + " and " + humText;
  }
}

/* -------------------------------------------------------------------------- */
/* Data Processing                                                            */
/* -------------------------------------------------------------------------- */

// Processes raw sensor data into a formatted weather forecast string.
/*
This function takes the raw sensor data, checks for validity, and constructs a human-readable weather forecast string.
*/
String processData(const SensorData &data)
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

  out += " || ";

  // LTR390
  if (!isnan(data.ltrALS) && !isnan(data.ltrUVS))
  {
    float ltrLux = 0.6 * data.ltrALS / (3.0 * 1.0);
    float ltrUVIndex = computeLTRUVIndex(data.ltrUVS, ltr.getGain(), ltr.getResolution());

    out += "Light: " + String(ltrLux, 1) + " lux";
    out += " | UV Index: " + String(ltrUVIndex, 1) + " (" + getUVCategory(ltrUVIndex) + ")";

    debugOutputSTR(out);
  }
  else
  {
    out += "LTR390: invalid";
  }

  /* ===================== "METSERVICE" ADDITION ===================== */

  WeatherModel w = buildWeatherModel(data);

  float ltrLux = 0.6 * data.ltrALS / (3.0 * 1.0);
  String sky = getSky(ltrLux);

  String temp = getTemp(w.temp);

  String hum = getHumidity(w.humidity);

  String precip = getPrecip(w.humidity, w.pressureTrend);

  
  String metServiceText = "";

  int style = random(0, 3);

  if (style == 0)
  {
    metServiceText = String(sky) + ", " + temp + " " + hum + " " + precip;
  }
  else if (style == 1)
  {
    metServiceText = String(sky) + " with " + temp + " conditions " + hum;
  }
  else
  {
    metServiceText = "Sunny with " + String(temp) + " and " + hum;
  }

  out += " || MET: " + metServiceText;

  return out;
}

/* -------------------------------------------------------------------------- */
/* Web Server & WebSocket Handling                                            */
/* -------------------------------------------------------------------------- */

// WebSocket Event Handler
/*
This function is called whenever a WebSocket event occurs (e.g., a client connects, disconnects, or sends a message).
It checks the type of event and processes incoming text messages that start with "command:".
*/
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
/*
This just sends alll data passed into the function to all connected WebSocket clients.
It also prints the data to the serial console if debug mode is enabled.
*/
void sendToWebsite(String data)
{
  if (DEBUG)
  {
    debugOutputSTR("WS send: " + data);
  }

  webSocket.broadcastTXT(data);
}

// HTTP handler for root page + Web page content
/*
This function serves the main web page when a client connects to the root URL ("/").
It constructs an HTML page with embedded CSS and JavaScript.
The page displays live sensor data and includes an input field for sending commands back to
 the server via WebSockets. The JavaScript code establishes a WebSocket connection to receive
 real-time updates and send user commands to the server.
*/
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

// Initializes the TFT display, sets rotation, and performs a quick color test if debug mode is enabled.
/*
This function is responsible for setting up the TFT display. It powers on the display,
 initializes it with the correct dimensions, and sets the orientation.
If debug mode is enabled, it also performs a quick color test by filling the screen with red, green,
 and blue colors sequentially to verify that the display is working correctly.
*/
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

    // Quick color test
    tft.fillScreen(ST77XX_RED);
    delay(500);
    tft.fillScreen(ST77XX_GREEN);
    delay(500);
    tft.fillScreen(ST77XX_BLUE);
    delay(500);
  }
}

/* -------------------------------------------------------------------------- */
/* Sensor Initialization                                                      */
/* -------------------------------------------------------------------------- */

// Initializes the BMP280 sensor and checks if it is present on the I2C bus.
void initBMP280()
{
  if (bmp.begin(0x76)) {
    infoOutput("BMP280 found");
    bmpPresent = true;
  } else if (bmp.begin(0x77)) {
    infoOutput("BMP280 found.");
    bmpPresent = true;
  } else {
    infoOutput("BMP280 not found.");
    bmpPresent = false;
  }
}

// Initializes the AHT20 sensor and checks if it is present on the I2C bus.
void initAHTX0()
{
  if (aht.begin()) {
    infoOutput("AHT20 found.");
  } else {
    infoOutput("[ERROR] AHT20 not found.");
  }
}

// Initializes the LTR390 sensor with specific settings for gain, resolution, and thresholds.
/*
This function attempts to initialize the LTR390 sensor and configures it for UV sensing mode.
It sets a moderate gain and resolution for quicker readings, and defines thresholds for light and UV levels
*/

/* 
 Here lies many hours of my life trying to get the LTR390 working, so I hope you appreciate
 this function.

 As well as the many hours, it also caused me physical pain in the form of electrical shocks 
 caused by the breadboard, which I will not forget until the day I die.

 int numberOfHoursSpentOnLTR390 = 6; 
 approximate, but only he who knows
*/
void initLTR390()
{
  if (!ltr.begin()) {
    infoOutput("[ERROR] LTR390 not found.");
  } else {
    infoOutput("LTR390 found.");
    ltr.enable(false);
    ltr.setMode(LTR390_MODE_UVS);

    // Use a faster resolution and moderate gain for quicker reads by default
    ltr.setGain(LTR390_GAIN_3);
    ltr.setResolution(LTR390_RESOLUTION_16BIT);
    ltr.setThresholds(100, 1000);
    ltr.enable(true);
    // Print configuration for debugging
    String cfg = "LTR cfg - mode:" + String(ltr.getMode()) + " gain:" + String(ltr.getGain()) + " res:" + String(ltr.getResolution());
    debugOutputSTR(cfg);
  }
}

// Quick I2C scanner to help debug wiring/address issues
/*
This function scans the I2C bus for connected devices and prints their addresses to the serial monitor.
It iterates through all possible I2C addresses (1 to 126) and attempts to communicate with each one.
If a device responds, its address is printed in hexadecimal format.
*/
void scanI2C()
{
  infoOutput("Scanning I2C bus...");
  byte count = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("  Found I2C device at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      count++;
    }
  }
  if (count == 0) infoOutput("No I2C devices found.");
  else infoOutput(String(count) + " I2C device(s) found.");
}


/* -------------------------------------------------------------------------- */
/* Setup                                                                      */
/* -------------------------------------------------------------------------- */

void setup()
{
  /* Initialize serial communication */
  Serial.begin(115200);
//  while (!Serial) {}
  delay(1000);

  /* Initialize the TFT display */
  initDisplay();

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20, 60);
  tft.println("Starting...");

  /* Wi-Fi Access Point setup */
  infoOutput("-- WIFI SETUP START --");

  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true, true);
  delay(1500);

  WiFi.mode(WIFI_AP);
  delay(1000);

  bool ok = WiFi.softAP(SSID, PASSWORD, 6, 0, 4);

  if (DEBUG && ok) {
    debugOutputSTR("AP STARTED");

    debugOutputSTR("AP SSID: " + String(SSID));
    debugOutputSTR("AP Password: " + String(PASSWORD));

    debugOutputSTR("AP IP: " + WiFi.softAPIP().toString());
  }
  else if (!ok)
  {
    debugOutputSTR("[ERROR] Failed to start AP");
  }

  // Web server routes
  server.on("/", handleRoot);
  server.begin();

  // WebSocket setup
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  /* Pin initialization */
  pinMode(BUTTON_D0, INPUT_PULLUP);
  pinMode(BUTTON_D1, INPUT_PULLDOWN);
  pinMode(BUTTON_D2, INPUT_PULLDOWN);

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);
  
  if(DEBUG){
    // show I2C devices to help debug wiring/address issues
    scanI2C();
  }

  /* Sensor initialization */
  initBMP280();
  initAHTX0();
  initLTR390();
}

// Display text with max X words and Y chars per line
/*
This function takes a long message and formats it to fit within specified character
 and word limits per line on the TFT display.
It splits the message into words and builds lines while ensuring that neither the
 character limit nor the word limit is exceeded. 
It then prints each line to the display with proper spacing.
*/
void displayFormattedMessage(String message, int startX, int startY, int lineHeight, int maxCharsPerLine, int maxWordsPerLine)
{
  // Split message into words
  String words[50];
  int wordCount = 0;
  String currentWord = "";
  
  for (int i = 0; i < message.length(); i++)
  {
    if (message[i] == ' ')
    {
      if (currentWord.length() > 0)
      {
        words[wordCount++] = currentWord;
        currentWord = "";
      }
    }
    else
    {
      currentWord += message[i];
    }
  }
  if (currentWord.length() > 0)
    words[wordCount++] = currentWord;
  
  int currentY = startY;
  int charsOnLine = 0;
  int wordsOnLine = 0;
  tft.setCursor(startX, currentY);
  
  for (int i = 0; i < wordCount; i++)
  {
    int wordLength = words[i].length();
    int spaceNeeded = (wordsOnLine > 0) ? 1 : 0;  // space before word if not first
    
    // Check if we need to move to next line (hit word limit OR char limit)
    if (wordsOnLine >= maxWordsPerLine || (charsOnLine + spaceNeeded + wordLength > maxCharsPerLine && wordsOnLine > 0))
    {
      currentY += lineHeight;
      tft.setCursor(startX, currentY);
      charsOnLine = 0;
      wordsOnLine = 0;
    }
    
    // Add space if not first word on line
    if (wordsOnLine > 0)
    {
      tft.print(" ");
      charsOnLine++;
    }
    
    tft.print(words[i]);
    charsOnLine += wordLength;
    wordsOnLine++;
  }
}

// Update the TFT display with sensor data and forecast
/*
This function takes the latest sensor data, processes it into a weather forecast,
 and updates the TFT display accordingly. 
It handles different display pages (home screen, forecast screen) and formats the output for readability. 
The home screen shows basic info and the forecast screen shows a detailed weather
 message based on the sensor readings.
*/
void updateTFT(const SensorData &data)
{
  tft.fillScreen(ST77XX_BLACK);
  
  if (currentPage == PAGE_HOME)
  {
    // Show the normal home screen
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);
    tft.setCursor(10, 10);
    tft.println("Weather");

    tft.setCursor(10, 40);
    tft.println("Station");

    tft.setTextSize(2);
    tft.setCursor(10, 70);
    tft.setTextColor(ST77XX_CYAN);
    tft.print("IP: ");
    tft.println(WiFi.softAPIP());

    tft.setTextSize(2);
    tft.setCursor(10, 110);
    tft.setTextColor(ST77XX_GREEN);
    tft.print("Made by Felix");

    return;   // Don't draw the normal screen
  }

    if (currentPage == PAGE_FORECAST)
  {
    // Show ONLY the weather message
    WeatherModel w = buildWeatherModel(data);
    const String met = buildTFTForecastMessage(data, w);

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Forecast");

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(false);
    displayFormattedMessage(met, formatMessageOptions[0], formatMessageOptions[1], formatMessageOptions[2], formatMessageOptions[3], formatMessageOptions[4]);

    return;   // Don't draw the normal screen
  }

  tft.setTextWrap(false);
/*
  // Header
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sensor Data");
*/

  // ===== BMP =====
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("Temp & Pressure: ");

  tft.setCursor(10, 30);

  if (!isnan(data.bmpTemp))
  {
    tft.print(data.bmpTemp);
    tft.print("C ");
    tft.print(data.bmpPressure);
    tft.print("hPa");
  }
  else tft.print("N/A");

  // ===== AHT =====
  tft.setCursor(10, 50);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("Temp2 & Humidity: ");

  tft.setCursor(10, 70);
  if (!isnan(data.ahtTemp))
  {
    tft.print(data.ahtTemp);
    tft.print("C ");
    tft.print(data.ahtHumidity);
    tft.print("%");
  }
  else tft.print("N/A");

  // ===== LIGHT =====
  float lux = NAN;
  if (!isnan(data.ltrALS))
    lux = 0.6 * data.ltrALS / 3.0;

  tft.setCursor(10, 90);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Sky: ");

  if (!isnan(lux))
    tft.print(getSky(lux));
  else
    tft.print("N/A");

  // ===== UV =====
  tft.setCursor(10, 110);
  tft.print("UV: ");

  if (!isnan(data.ltrUVS))
  {
    float uv = computeLTRUVIndex(data.ltrUVS, ltr.getGain(), ltr.getResolution());
    tft.print(uv, 1);
  }
  else tft.print("N/A");
}

// Reads sensors, processes data, sends to website, and updates TFT display
/*
This function performs the following steps:
1. Reads data from the BMP280, AHT20, and LTR390 sensors.
2. Processes the raw sensor data into a formatted string (including a weather forecast).
3. Sends the formatted data to connected WebSocket clients.
4. Updates the TFT display with the latest sensor data and forecast.
*/
void updateSensors()
{
  SensorData data = { NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN };

  // BMP280 (use initialized flag; don't call begin() repeatedly)
  if (bmpPresent)
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

  /* 
  Look where the function definition: initLTR390(); is.
  As of writing this, it is line 465 to 470.
  */

  // LTR390
  if (ltr.enabled())
  {
    // ALS (ambient light) - set config again to ensure the mode is active
    ltr.setMode(LTR390_MODE_ALS);
    ltr.setGain(LTR390_GAIN_3);
    ltr.setResolution(LTR390_RESOLUTION_16BIT);
    ltr.enable(true);
    debugOutputSTR("LTR config before ALS: mode=" + String(ltr.getMode()) + " gain=" + String(ltr.getGain()) + " res=" + String(ltr.getResolution()) + " enabled=" + String(ltr.enabled()));
    delay(100);

    unsigned long start = millis();
    bool gotALS = false;

    while (millis() - start < newDataInterval) {
      if (ltr.newDataAvailable()) { gotALS = true; break; }
      delay(25);
    }
    if (gotALS) {
      uint32_t alsRaw = ltr.readALS();
      data.ltrALS = alsRaw;
      debugOutputSTR("LTR ALS raw: " + String(alsRaw));
    } else {
      data.ltrALS = NAN;
      data.ltrLux = NAN;
      debugOutputSTR("LTR ALS: no data");
    }

    // UVS (UV light)
    ltr.setMode(LTR390_MODE_UVS);
    ltr.setGain(LTR390_GAIN_18);
    ltr.setResolution(LTR390_RESOLUTION_16BIT);
    ltr.enable(true);
    debugOutputSTR("LTR config before UVS: mode=" + String(ltr.getMode()) + " gain=" + String(ltr.getGain()) + " res=" + String(ltr.getResolution()) + " enabled=" + String(ltr.enabled()));
    delay(100);

    start = millis();
    bool gotUV = false;

    while (millis() - start < newDataInterval) {
      if (ltr.newDataAvailable()) { gotUV = true; break; }
      delay(25);
    }
    if (gotUV) {
      uint32_t uvRaw = ltr.readUVS();
      data.ltrUVS = uvRaw;
      debugOutputSTR("LTR UVS raw: " + String(uvRaw));
    } else {
      data.ltrUVS = NAN;
      data.ltrUVIndex = NAN;
      debugOutputSTR("LTR UVS: no data");
    }
  }
  else
  {
    data.ltrALS = NAN;
    data.ltrUVS = NAN;
    data.ltrLux = NAN;
    data.ltrUVIndex = NAN;
  }

  // Process everything
  String output = processData(data);
  sendToWebsite(output);

  // Update TFT display (same sensor snapshot)
  updateTFT(data);
}

// Serial input handling for debug commands
/*
This allows you to type commands into the Serial Monitor to trigger actions in the program.
For example, typing "update" will call updateSensors() immediately, and typing
 "send" followed by a message will send that message to all connected WebSocket clients.

It only runs with DEBUG mode enabled, and it has access to Serial monitor
*/
void handleCommands()
{
  if (!Serial.available() && !DEBUG)
  {
    infoOutput("Serial input available, but DEBUG mode is off so ignoring...");
  }
  else{
    debugOutputSTR("Serial input detected");
  
    String serialInput = Serial.readString();
    if(serialInput == "update")
    {
      updateSensors();
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
  }
}

// Button state logic
/* 
This is where the page switching logic happens. 
It reads the state of the buttons and updates the currentPage variable accordingly. 
When a button is pressed, it also calls updateSensors()
 to refresh the display immediately with the new page's content.
*/
void handleButtons()
{
  byte D0_state = digiReadSensorData("D0", BUTTON_D0);
  byte D1_state = digiReadSensorData("D1", BUTTON_D1);
  byte D2_state = digiReadSensorData("D2", BUTTON_D2);

  if (D0_state == LOW && currentPage != PAGE_HOME)
  {
    currentPage = PAGE_HOME;
  }

  if (D1_state == HIGH && currentPage != PAGE_FORECAST)
  {
    currentPage = PAGE_FORECAST;
    updateSensors();  // Refresh display immediately when changing page
  }

  if (D2_state == HIGH && currentPage != PAGE_DATA)
  {
    currentPage = PAGE_DATA;
    updateSensors();  // Refresh display immediately when changing page
  }
}

/* -------------------------------------------------------------------------- */
/* Main Loop                                                                  */
/* -------------------------------------------------------------------------- */

void loop()
{
  // web server and websocket handling
  /* 
  Updates the web server and WebSocket connections. 
  This is necessary to keep the server responsive and to handle incoming messages from clients.
  */
  server.handleClient();
  webSocket.loop();

  // Sensor reading and processing every 10 seconds
  /*
  Every 10 seconds, the program reads data from the sensors, processes it 
   into a weather forecast, and sends it to connected clients.
  */
  if (millis() - lastPrint > sensorUpdateInterval)
  {
    lastPrint = millis();
    updateSensors();
  }


  // Serial input handling for debug commands
  handleCommands();

  // Button state handling for page switching
  handleButtons();
}