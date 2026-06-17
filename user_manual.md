# Weather Station User Manual

## Overview
This is an ESP32-based weather station that collects real-time atmospheric and light data from multiple sensors. The device displays weather information on a built-in TFT screen and allows users to access data via a web interface through WiFi.

### Key Features
- **Real-time Weather Forecasting**: Generates human-readable weather forecasts based on sensor data
- **Multi-sensor Support**: Temperature, humidity, pressure, UV light, and ambient light sensors
- **Web Interface**: Access real-time data remotely via WiFi
- **Interactive Display**: Three different information screens with button navigation
- **UV Index Tracking**: Monitors UV exposure levels
- **Pressure Trend Analysis**: Tracks atmospheric pressure changes

---

## Hardware Setup

### Components
The weather station uses the following sensors and components:
- **Adafruit ESP32-S3 Reverse TFT Feather** - Main microcontroller with built-in display
- **Adafruit BMP280** - Temperature and pressure sensor
- **Adafruit AHT20** - Temperature and humidity sensor
- **Adafruit LTR390** - UV light and ambient light sensor
- **Breadboard and connecting wires** - For sensor integration

### Sensor Specifications
- **BMP280**: Measures temperature (°C) and atmospheric pressure (hPa)
- **AHT20**: Measures temperature (°C) and relative humidity (%)
- **LTR390**: Measures UV index and ambient light (lux)

---

## Getting Started

### Initial Power-Up
1. Connect the ESP32 to power via USB-C cable
2. The device will initialize all connected sensors
3. If sensors are detected, it will read N/A in the sensor data screen

### Understanding the Display
The 240x135 pixel TFT display shows:
- Sensor status at startup
- Current weather conditions
- Detailed sensor readings
- WiFi connection status and IP address

---

## User Interface

### Navigating Between Screens
Use the three buttons on the device (D0, D1, D2) to navigate between information screens:

#### **Home Screen (Button D0)**
- Displays the IP address for WiFi connection
- Shows current WiFi network status
- Provides quick visual reference of the device

#### **Forecast Screen (Button D1)**
- Shows an algorithmically generated weather forecast sentence
- Includes:
  - **Sky conditions**: Clear, sunny, partly cloudy, cloudy, or overcast
  - **Temperature description**: Very cold, cold, cool, mild, warm, hot, or very hot
  - **Humidity description**: Very dry, dry, comfortable, humid, or very humid
  - **Precipitation likelihood**: Based on humidity and pressure trends
  - **Pressure trend**: Shows if conditions are stable, improving, becoming unsettled, or deteriorating

#### **Sensor Data Screen (Button D2)**
- Displays raw sensor readings:
  - **BMP280**: Temperature (°C) and Pressure (hPa)
  - **AHT20**: Temperature (°C) and Humidity (%)
  - **LTR390**: Ambient light (lux) and UV Index
- Shows UV category (Low, Moderate, High, Very High, or Extreme)
- Updates every few seconds

---

## WiFi Connectivity

### Connecting to the Web Interface
The weather station creates its own WiFi network for remote monitoring:

1. **On your device (phone, tablet, laptop):**
   - Find WiFi network: `Code-ESP32`
   - Enter password: `12345678`

2. **Access the web interface:**
   - Open a web browser
   - Type the IP address shown on the Home Screen (typically `192.168.4.1`)
   - Press Enter to load the dashboard

3. **Real-time updates:**
   - The web interface updates automatically via WebSocket connection
   - Data refreshes every few seconds
   - All sensor values display in real-time

### IP Address
The device typically uses IP address `192.168.4.1`, but you can verify the exact address by:
- Checking the Home Screen display on the device
- Looking at your WiFi network details if the address differs

---

## Interpreting Weather Data

### Temperature Classification
- Very Cold: Below 5°C
- Cold: 5–12°C
- Cool: 12–18°C
- Mild: 18–24°C
- Warm: 24–30°C
- Hot: 30–36°C
- Very Hot: Above 36°C

### Humidity Classification
- Very Dry: Below 20%
- Dry: 20–40%
- Comfortable: 40–60%
- Humid: 60–80%
- Very Humid: Above 80%

### Sky Conditions (based on ambient light)
- Overcast: Below 50 lux
- Cloudy: 50–500 lux
- Partly Cloudy: 500–3000 lux
- Sunny: Above 3000 lux

### UV Index Categories
- **Low** (0–2): Minimal UV exposure risk
- **Moderate** (3–5): Moderate UV exposure; protection recommended
- **High** (6–7): High UV exposure; strong protection needed
- **Very High** (8–10): Very high UV exposure; take precautions
- **Extreme** (11+): Extreme UV exposure; avoid sun exposure

### Precipitation Prediction
The device predicts precipitation likelihood based on:
- Humidity levels (higher humidity = more likely rain)
- Pressure trends (falling pressure = worsening conditions)
- Conditions are considered "improving" with rising pressure
- Conditions are "deteriorating" with falling pressure

---

## Troubleshooting

### Display Issues
- **Blank or distorted display:**
  - Power cycle the device (unplug and reconnect USB)
  - Ensure the TFT is properly connected to the ESP32

### Sensor Problems
- **Sensors showing "Not found":**
  - Check wiring connections on the breadboard
  - Ensure sensor addresses match device configuration
  - Try power cycling the device

### WiFi Connection Issues
- **Can't connect to "Code-ESP32" network:**
  - Ensure the ESP32 is powered and operational
  - Check that WiFi is enabled (verify on Home Screen)
  - Try disconnecting and reconnecting to the network

- **IP address doesn't work:**
  - Try the default address `192.168.4.1`
  - If unsuccessful, check the Home Screen for the correct IP
  - Ensure you're connected to the `Code-ESP32` network

- **Web interface not loading:**
  - Verify the ESP32 is powered on
  - Confirm WiFi connection status on the Home Screen
  - Try refreshing the page in your browser
  - Wait 5–10 seconds for the initial WebSocket connection

### Erratic Readings
- **Fluctuating sensor values:**
  - This may indicate a loose connection; check wiring
  - Ensure the breadboard connections are secure
  - Move the device away from direct heat or electronic interference

---

## Maintenance and Care

### Sensor Maintenance
- Keep sensors clean and dust-free
- Avoid exposing sensors to extreme temperatures or moisture
- The LTR390 light sensor is sensitive; protect from direct heat

### Software Updates
- The device uses PlatformIO for firmware management
- Refer to the project repository for latest firmware versions
- Updates are uploaded via USB-C cable

### Data Logging
- The web interface displays real-time data but does not log historical data
- To track trends over time, consider recording screenshots or using browser developer tools

---

## Advanced Features

### Debug Mode
Developers can enable debug mode in the source code to see:
- Detailed sensor readings in Serial Monitor
- WebSocket communication logs
- System status messages

To enable debug mode:
1. Connect device via USB to a computer
2. Open the code and set `DEBUG = true`
3. Upload the code to the ESP32
4. Open Arduino IDE Serial Monitor to view debug output

### Modifying Weather Descriptions
The device uses predefined phrases for weather conditions. To customize:
1. Edit the weather phrase arrays in the source code
2. Modify temperature, humidity, sky, or precipitation categories
3. Recompile and upload to the ESP32

---

## Technical Details

### Communication Protocol
- **WiFi**: ESP32 AP mode (Access Point)
- **Web Server**: HTTP with WebSocket support on port 81
- **Sensors**: I2C protocol via pins GPIO 3 (SDA) and GPIO 4 (SCL)

### Refresh Rates
- Display updates: Every 2–5 seconds
- Web interface updates: Via WebSocket, nearly real-time
- Weather forecast recalculation: Every update cycle

### Power Consumption
- Typical: ~500mA during operation
- Higher when WiFi is active
- USB-powered (5V, 2A recommended)

---

## Safety and Warnings

- **UV Exposure**: Do not stare at light sensors directly
- **Power Safety**: Use a certified USB power adapter with at least 2A output
- **Temperature Range**: Operate the device between 0–40°C
- **Moisture Protection**: Avoid exposing the device to rain or condensation

---

## Support and Documentation

For technical details, visit:
- Project GitHub Repository
- Adafruit Sensor Libraries Documentation
- Arduino IDE Documentation

For issues or questions:
- Check the troubleshooting section above
- Review source code comments for implementation details
- Contact the project developers