# Weather Station Project Information

## Project Brief

**Course**: NCEA Level 2 Digital Technologies (AS 91897 & AS 91896)  
**Year**: 2026  
**Student**: Felix Love (NSN: 145498404)

### Original Brief
Develop a weather station project using multiple sensors and outputs to showcase skills and solve a real-world problem. Requirements included:
- At least one sensor input and one button
- Interactive display (built-in TFT screen)
- WiFi/Bluetooth connectivity for additional input
- LED indicators for visual feedback
- Real-time data processing and display

### Project Solution
Created an intelligent weather station that:
- Reads multiple atmospheric sensors continuously
- Generates natural-language weather forecasts
- Displays data on a TFT screen with multi-page interface
- Provides remote access via WiFi web interface
- Adapts visual display based on weather conditions

---

## Development Approach

### Design Decisions

#### 1. **Multi-Sensor Strategy**
- **BMP280**: Temperature and pressure for weather trend analysis
- **AHT20**: Independent temperature reading and humidity for moisture-based forecasting
- **LTR390**: UV and ambient light for sky condition assessment
- **Rationale**: Multiple sensors reduce reliance on single faulty input and enable cross-validation

#### 2. **Weather Forecasting Algorithm**
Rather than simple data display, the system generates descriptive weather forecasts using sensor inputs:
- **Sky conditions** derived from light levels (lux)
- **Temperature classification** using thermal thresholds
- **Humidity-based precipitation prediction**
- **Pressure trend analysis** for condition forecasting
- **Randomized sentence structure** for natural-sounding output

#### 3. **Display Architecture**
Implemented a three-screen interface:
- **Home Screen**: Network information and device status
- **Forecast Screen**: Human-readable weather prediction
- **Sensor Data Screen**: Raw numerical readings for verification
- **Rationale**: Balances user experience (home/forecast) with technical transparency (sensor data)

#### 4. **Connectivity Options**
- **WiFi Access Point Mode**: Device creates its own network rather than connecting to existing WiFi
- **WebSocket Protocol**: Real-time bidirectional communication with web clients
- **Dual Interface**: Both TFT display and web dashboard show same data
- **Rationale**: AP mode avoids network dependency; WebSockets provide responsive updates

#### 5. **Modular Code Structure**
Code organized into logical sections:
- Debug/Info functions for development
- Data processing functions for forecasting
- Web server/WebSocket handlers
- TFT display management
- Sensor initialization
- Main setup/loop

---

## Technical Implementation

### Hardware Components
| Component | Model | Purpose | I2C Address |
|-----------|-------|---------|------------|
| Microcontroller | Adafruit ESP32-S3 Reverse TFT Feather | Main processing unit with TFT display | N/A |
| Temperature/Pressure | Adafruit BMP280 | Atmospheric monitoring | 0x76 or 0x77 |
| Temperature/Humidity | Adafruit AHT20 | Environmental monitoring | 0x38 |
| UV/Light Sensor | Adafruit LTR390 | Solar radiation monitoring | 0x53 |

### Software Stack
- **Language**: C++ (Arduino framework)
- **IDE**: PlatformIO / Arduino IDE
- **Key Libraries**:
  - `WiFi.h` & `WebServer.h` - Network connectivity
  - `WebSocketsServer.h` - Real-time client communication
  - `Adafruit_GFX.h` & `Adafruit_ST7789.h` - Display rendering
  - Sensor libraries (BMP280, AHT20, LTR390) for I2C communication

### Pin Configuration
| GPIO | Function | Details |
|------|----------|---------|
| D0 | Button Input | Page navigation (Home screen) |
| D1 | Button Input | Page navigation (Forecast screen) |
| D2 | Button Input | Page navigation (Sensor data screen) |
| GPIO 3 (SDA) | I2C Data | Sensor communication |
| GPIO 4 (SCL) | I2C Clock | Sensor communication |

### Key Algorithms

#### Weather Classification System
Uses sensor readings to categorize conditions:

```
Sky Conditions (based on lux):
- Overcast: < 50 lux
- Cloudy: 50–500 lux
- Partly Cloudy: 500–3000 lux
- Sunny: > 3000 lux

Temperature Categories (°C):
- Very Cold: < 5°C
- Cold: 5–12°C
- Cool: 12–18°C
- Mild: 18–24°C
- Warm: 24–30°C
- Hot: 30–36°C
- Very Hot: > 36°C

Humidity Categories (%):
- Very Dry: < 20%
- Dry: 20–40%
- Comfortable: 40–60%
- Humid: 60–80%
- Very Humid: > 80%
```

#### Pressure Trend Analysis
Calculates atmospheric pressure change since last reading to predict weather improvements/deterioration:
- Rising pressure (> +1 hPa) = Conditions improving
- Falling pressure (< -2 hPa) = Conditions deteriorating
- Stable (±1 to 2 hPa) = Conditions stable

#### UV Index Computation
Raw UV counts from LTR390 converted to UV Index using empirical calibration constant:
```
UV Index = Raw UV Count / 23.0
```
Categories: Low (0–2), Moderate (3–5), High (6–7), Very High (8–10), Extreme (11+)

---

## Project Management

### Development Timeline

#### Phase 1: Planning & Research (Week 1–2)
- ✅ Analyzed project brief and rubric requirements
- ✅ Selected appropriate hardware components
- ✅ Researched Adafruit sensor libraries
- ✅ Designed system architecture

#### Phase 2: Hardware Setup & Sensor Integration (Week 3–4)
- ✅ Assembled breadboard with I2C sensors
- ✅ Tested individual sensor libraries
- ✅ Debugged I2C communication issues (LTR390 issues documented in code)
- ✅ Verified sensor accuracy and response times

#### Phase 3: Core Programming (Week 5–7)
- ✅ Implemented TFT display initialization and rendering
- ✅ Created WiFi access point and web server
- ✅ Built WebSocket communication layer
- ✅ Implemented multi-page display navigation
- ✅ Developed weather forecasting algorithm

#### Phase 4: Testing & Refinement (Week 8–9)
- ✅ Tested all sensors for accuracy and reliability
- ✅ Validated weather forecast logic across temperature ranges
- ✅ Tested WiFi connectivity and web interface
- ✅ Debugged edge cases and boundary conditions
- ✅ Optimized code for performance

#### Phase 5: Documentation & Finalization (Week 10)
- ✅ Created user manual with step-by-step instructions
- ✅ Documented code with inline comments
- ✅ Recorded video evidence of testing
- ✅ Finalized project submission

---

## Testing Strategy

### Unit Testing (Component Verification)
- ✅ Tested each sensor independently for correct readings
- ✅ Verified I2C communication for address conflicts
- ✅ Tested TFT display rendering and text wrapping
- ✅ Tested WiFi AP mode connection
- ✅ Verified WebSocket message send/receive

### Integration Testing
- ✅ All three sensors reading simultaneously
- ✅ Data flows correctly through processing pipeline
- ✅ Display updates reflect sensor changes
- ✅ Web interface receives and displays live data
- ✅ Button navigation switches between all pages smoothly

### Boundary Testing
- ✅ Very high/low temperature values
- ✅ Extreme humidity levels (0–100%)
- ✅ Light levels from total darkness to direct sunlight
- ✅ Pressure variations (weather fronts)
- ✅ Multiple simultaneous web clients

### Real-World Testing
- ✅ Outdoor testing in various weather conditions
- ✅ Testing in different lighting environments
- ✅ Multiple devices connecting to web interface
- ✅ Power cycling resilience

### Test Coverage
- **Expected Cases**: ✅ All pass
- **Boundary Cases**: ✅ Handled gracefully
- **Invalid Cases**: ✅ Error messages displayed

---

## Implementation Highlights

### Advanced Techniques Used

#### 1. **Raw String Literals** (C++)
```cpp
String page = R"rawliteral(HTML/JS content)rawliteral";
```
Embedded entire HTML/CSS/JavaScript dashboard inline

#### 2. **Non-Core Libraries**
- Adafruit Graphics Library for text rendering
- WebSocket protocol for real-time communication
- Multiple I2C device drivers

#### 3. **Struct-Based Data Organization**
```cpp
struct SensorData { float bmpTemp, bmpPressure, ahtTemp, ... };
struct WeatherModel { float temp, humidity, light, pressure, ... };
```
Organized complex sensor data into manageable structures

#### 4. **Dynamic Text Formatting**
Implemented `displayFormattedMessage()` to intelligently wrap text across multiple lines with word boundaries

#### 5. **Gain/Resolution Computation** (LTR390)
Custom calibration factors for different sensor gain and resolution settings:
```cpp
float getLTRGainFactor(ltr390_gain_t gain)
int getLTRResolutionBits(ltr390_resolution_t res)
```

#### 6. **Error Handling & Sensor Presence Flags**
- Graceful degradation when sensors unavailable
- Detection of multiple I2C addresses for sensors
- NaN checks throughout data pipeline

---

## Challenges & Solutions

### Challenge 1: LTR390 Integration (6+ hours debugging)
**Problem**: Sensor not initializing or providing invalid readings
**Solution**: 
- Implemented I2C bus scanner for debugging
- Tested multiple gain/resolution configurations
- Created empirical calibration formula for UV index
- Added detailed debug output in sensor initialization

### Challenge 2: Text Display on Small Screen
**Problem**: Weather forecast text overflow and poor readability
**Solution**:
- Implemented word-wrapping algorithm
- Set maximum words per line and characters per line
- Tested with longest possible forecasts

### Challenge 3: WiFi Access Point Stability
**Problem**: Intermittent WebSocket disconnections
**Solution**:
- Implemented automatic reconnection in JavaScript
- Added exponential backoff for retry attempts
- Verified adequate power supply (2A USB)

### Challenge 4: Multi-Sensor Data Synchronization
**Problem**: Sensors read at different rates, causing forecast inconsistencies
**Solution**:
- Collected all sensor readings at once in `updateSensors()`
- Used struct to pass data as single unit
- Timestamp pressure readings for trend calculation

---

## Rubric Alignment

### PLANNING (AS 91897)
- ✅ Decomposed outcome into individual tasks
- ✅ Created development timeline with milestones
- ✅ Used version control (Git) with regular commits
- ✅ Addressed implications: Privacy (AP mode), Environmental impact (low power design)

### DOING (AS 91896)
- ✅ Used multiple data types (float, int, String, struct, enum)
- ✅ Implemented loops and conditional logic throughout
- ✅ Methods with parameters (functions with inputs/outputs)
- ✅ Non-core libraries: Adafruit libraries, WebSocket library
- ✅ Clear naming conventions: camelCase for methods/variables, CAPS for constants
- ✅ Well-structured code organized into logical sections
- ✅ Handles boundary cases (NaN checks, sensor missing, extreme values)

### TESTING (AS 91898)
- ✅ Trialled each component (sensors, display, WiFi)
- ✅ Tested against expected cases and boundary cases
- ✅ Compared alternatives and selected best options
- ✅ Testing driven improvements (LTR390 calibration, text rendering)
- ✅ Testing throughout development (Git history shows incremental testing)

### REFLECTION
- ✅ Documented learning from planning phase to final implementation
- ✅ Explained how testing and trialling improved program quality
- ✅ Identified areas of growth and future improvements

---

## Future Enhancement Ideas

1. **Data Logging**: Store historical readings to microSD card
2. **Threshold Alerts**: LED color changes based on weather severity
3. **Mobile App**: Native iOS/Android app instead of web dashboard
4. **Multi-Language**: Forecasts in multiple languages
5. **Machine Learning**: Improve forecast accuracy with ML model
6. **Cloud Integration**: Sync data with cloud weather API
7. **Solar Power**: Add solar charging for outdoor deployment
8. **MQTT Support**: Integrate with smart home systems

---

## Credits & Acknowledgements

### Teachers
- Mr Jihoon Park
- Mr Peter Yoon

### Inspiration & Guidance
- Zac Taylor

### Developer
- Felix Love

### AI Assistance
This project used GitHub Copilot and ChatGPT for:
- Bug fixing support (after attempting manual debugging)
- Code template generation
- Documentation assistance

All core code is original work; AI was used as a development tool for optimization and debugging.

---

## File Structure
### My File Structure:
```
Weather Station/
├── src/
│   └── main.cpp              # Main program (1145 lines)
├── include/                  # Header files
├── lib/                      # Custom libraries
├── platformio.ini            # PlatformIO configuration
├── user_manual.md            # User guide (for end users)
├── project_info.md           # This file (project documentation)
├── README.md                 # Project overview
├── testing/                  # Test files and logs
├── versions.txt              # Version history
└── temp_ltr390_ds.pdf        # LTR390 datasheet reference
```

### Minimum needed File Structure:
```
Weather Station/
├── src/
│   └── main.cpp              # Main program (1145 lines)
├── include/                  # Header files
├── lib/                      # Custom libraries
└── platformio.ini            # PlatformIO configuration
```

---

## System Requirements

### For Development
- PlatformIO IDE or Arduino IDE
- USB 2.0+ port for programming
- 2A USB power supply recommended

### For Deployment
- USB power adapter (5V, 2–3A recommended)
- Temperature: 0–40°C operating range
- Humidity: Non-condensing environment

---

## Known Limitations

1. **No Historical Data**: Real-time data only; no permanent storage
2. **Single AP Network**: Cannot connect to existing WiFi networks
3. **4 Client Limit**: WebSocket server limited to ~4 simultaneous connections
4. **Manual Calibration**: UV index requires empirical calibration per unit
5. **Display Resolution**: 240x135 pixels limits text size and amount

---

## Version History

See `versions.txt` for detailed changelog and version information.

---

**Last Updated**: June 2026  
**Status**: Complete and Functional  
**License**: Educational Use