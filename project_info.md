## 1. Overview
This project is an Arduino-based system written in C++ that controls hardware components (such as sensors, LEDs, motors, or displays). It is designed to be modular, configurable, and easy to extend.

## 2. Requirements

### Hardware
- Adafruit ESP32-S3 Reverse TFT Feather
- USB C cable for programming and power
- Required components:
  - Adafruit LTR390 UV Light Sensor - STEMMA QT / Qwiic
  - Adafruit AHT20 Temperature & Humidity Sensor
  - Breadboard

### Software
- Arduino IDE (latest version recommended)
- Required libraries (check project code for `#include` statements)

## 3. Installation

1. Download or clone the project files
2. Open the `.ino` or `.cpp` file in Arduino IDE
3. Connect your Arduino via USB
4. Select:
   - Board type (Tools → Board)
   - Port (Tools → Port)
5. Install required libraries via Library Manager
6. Click **Upload**

## 4. How It Works

The program follows this structure:

- `setup()`:
  - Runs once at startup
  - Initializes pins, sensors, and serial communication

- `loop()`:
  - Runs continuously
  - Reads inputs (sensors/buttons)
  - Processes data
  - Updates outputs (LEDs, motors, display)

## 5. Configuration

You may be able to adjust:

- Pin assignments in the header section of the code
- Timing intervals using `delay()` or `millis()`
- Threshold values for sensors
- Debug mode via Serial Monitor

## 6. Usage

1. Power the Arduino via USB or external supply
2. Observe startup behavior (LED blink / serial output)
3. Interact with sensors or inputs
4. Monitor output via:
   - TFT Display
   - Serial Monitor (Tools → Serial Monitor)

## 7. Troubleshooting

### Board not uploading
- Check correct board and port selected
- Try different USB cable
- Install drivers if needed

### Nothing happens
- Check wiring connections
- Ensure correct pin definitions in code
- Open Serial Monitor for debug output

### Erratic sensor readings
- Add delays or smoothing in code
- Check power stability
- Verify sensor wiring

## 8. Credits
- Mr Jihoon Park
- Mr Peter Yoon
- Zac Taylor
- Felix Love

## 9. Acknowledgements
This project *has* used AI - GitHub Copiolt and ChatGPT - for help in development. All code is my own, I have just used AI to help in the fixing of interesting bugs (which I had already tried fixing myself), and templating code.