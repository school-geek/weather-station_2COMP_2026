# Weather Station Project — Marked Rubric

This is a teacher-style marking version of your rubric based on the current project files and code in the workspace.

Sources used:
- [README.md](README.md)
- [project_info.md](project_info.md)
- [versions.md](versions.md)
- [user_manual.md](user_manual.md)
- [src/main.cpp](src/main.cpp)

Overall judgement:
- Planning: Achieved with Merit
- Doing: Achieved with Merit
- Testing: Achieved
- Reflection: Achieved with Merit
- Overall: Achieved with Merit

Note:
- The code shows strong evidence of advanced programming and project development.
- The main gaps for Excellence are the lack of a formal Gantt chart, stronger external video evidence, and a fully verified hardware test record.

---

# 1. PLANNING

**AS 91897 — Use advanced processes to develop a digital technologies outcome**

| Criteria | Evidence | A | M | E |
| --- | --- | --- | --- | --- |
| Decomposing the outcome into smaller components | The project has been broken into clear sections such as hardware setup, sensor integration, Wi-Fi/web interface, display handling, and testing. This is reflected in the project documentation and code structure. | ✅ |  |  |
| Using appropriate project management tools and techniques to plan the development of a digital technologies outcome | A development timeline, milestone phases, and version history are documented in the project files. A formal Gantt chart is not clearly present, so this is stronger than basic planning but not fully refined. | ✅ |  |  |
| Explaining relevant implications | The project discusses safety, maintenance, power use, and Wi-Fi/network privacy-related concerns in the documentation. |  | ✅ |  |
| Effectively using project management and version control tools and techniques to manage the development of a digital technologies outcome | Version history and repeated commits are clearly documented in [versions.md](versions.md), showing ongoing development and revision. |  | ✅ |  |
| Addressing relevant implications | The program avoids reliance on insecure or unnecessary network setup by using an access point and includes safety and maintenance notes in the user manual. |  | ✅ |  |

**Planning mark:** Achieved with Merit

---

# 2. DOING

**AS 91896 — Use advanced programming techniques to develop a computer program**

| Criteria | Evidence | A | M | E |
| --- | --- | --- | --- | --- |
| Writing code for a program that performs a specified task | The program reads sensor data, updates a TFT display, creates a Wi-Fi access point, hosts a web page, and sends live data through WebSockets. This clearly meets the brief. | ✅ |  |  |
| Loops and if statements are used in a well thought out way | The code uses loops for sensor waiting, message wrapping, and input handling. Conditional logic is used throughout for page switching, sensor validity checks, and weather classification. | ✅ |  |  |
| User input and program output is well thought out with respect to UI heuristics | The display has clear screens, readable labels, and a simple web interface. The output is presented in a user-friendly way. |  | ✅ |  |
| Methods are created that take parameters and/or give return values | Many functions take parameters and return values, including functions for weather logic, display formatting, UV classification, and sensor processing. | ✅ |  |  |
| Using non-core libraries | The program uses multiple non-core libraries, including Adafruit display and sensor libraries and WebSocket/WebServer libraries. | ✅ |  |  |
| Setting out the program code clearly and documenting the program with comments | The source code is well structured into sections and includes detailed comments for many functions. |  | ✅ |  |
| Names of classes, methods, variables are appropriate and descriptive | Names such as updateSensors(), buildWeatherModel(), displayFormattedMessage(), and currentPage are clear and relevant. |  | ✅ |  |
| Following common conventions for the chosen programming language | The code mostly uses camelCase naming and constants in uppercase, which is appropriate for Arduino/C++. | ✅ |  |  |
| Ensuring that the program is a well-structured, logical response to the specified task | The code is organised into functions for display, data processing, web handling, and sensor setup, making it logical and maintainable. |  | ✅ |  |
| Making the program flexible and robust | The program handles invalid sensor values, missing sensors, and edge cases using NaN checks and fallback output. |  | ✅ |  |
| Constants, variables, and derived values like random() are used in place of hard coded values | Threshold values, delays, and formatting constants are defined clearly rather than being repeated as magic numbers. |  | ✅ |  |

**Doing mark:** Achieved with Merit

---

# 3. TESTING

| Criteria | Evidence | A | M | E |
| --- | --- | --- | --- | --- |
| Trialling the components of the digital technologies outcome | The project documents show testing of sensors, display output, Wi-Fi, and the web interface. | ✅ |  |  |
| Testing that the digital technologies outcome functions as intended | The program clearly shows functional components for sensing, updating, and displaying information. | ✅ |  |  |
| Trialling multiple components and/or techniques and selecting those which are most suitable | The project shows comparison and adjustment of sensor settings and display solutions, especially for the LTR390 calibration and text layout. |  | ✅ |  |
| Using information appropriately from testing and trialling to improve the functionality of the digital technologies outcome | The code and documentation reflect improvements made after debugging the LTR390, TFT formatting, and display logic. |  | ✅ |  |
| Testing and debugging the program to ensure that it works on a sample of expected cases | The code appears logically correct and editor diagnostics report no errors in [src/main.cpp](src/main.cpp). | ✅ |  |  |
| Video evidence of testing on the program to prove it can handle a range of expected input | No video evidence is present in the workspace, so this is not yet strong enough for Merit or Excellence in this area. | ✅ |  |  |
| Video evidence of testing process is organised and clear | This is not yet clearly evidenced in the current project files. |  |  |  |
| Testing is done on the finished program to prove it can handle a range of expected and boundary input | The code includes boundary handling for missing sensor values and invalid data, which is good, but stronger recorded evidence would improve this further. | ✅ |  |  |
| No compile bugs in the program’s code | Editor diagnostics show no errors, but a full PlatformIO build could not be verified in this environment because the PlatformIO CLI was not available. | ✅ |  |  |

**Testing mark:** Achieved

---

# 4. REFLECTION

| Criteria | Evidence | A | M | E |
| --- | --- | --- | --- | --- |
| Discussing how the information from planning, testing and trialling of components assisted in the development of a high-quality outcome | The project documentation includes challenges, solutions, and improvement notes, showing that testing and debugging informed the final design. |  | ✅ |  |

**Reflection mark:** Achieved with Merit

---

## Final teacher-style comment

This is a strong and well-developed weather station project that shows clear evidence of planning, programming skill, and problem solving. The code is structured well, uses multiple sensors and libraries, and includes a responsive TFT/web interface. The project demonstrates a good level of technical understanding and has strong documentation. To move into the Excellence range, the project would benefit from a formal Gantt chart, stronger evidence of ongoing testing through videos or screenshots, and a fully recorded hardware validation process.
