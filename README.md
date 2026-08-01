CAN-Based Vision-Assisted Smart Vehicle Headlight Control System using ESP32-CAM and CAN Bus
Bachelor Degree in Automobile Engineering Final Year Project
National Institute of Transport (NIT)
Author: Masoud M. Subi
Table of Contents
Overview
Project Objectives
System Architecture
System Description
System Operation
Safety Features
CAN Communication
Diagnostic Commands
Hardware Components
Software Requirements
Repository Structure
Installation Guide
System Testing
Future Improvements
Contributing
Author
License
Overview
The CAN-Based Vision-Assisted Smart Vehicle Headlight Control System is a distributed embedded automotive lighting prototype designed to improve driving safety by automatically controlling vehicle headlights according to ambient lighting conditions and approaching vehicle headlights.
Unlike conventional automatic lighting systems where sensing and control are performed within a single controller, this project adopts a distributed Electronic Control Unit (ECU) architecture connected through the Controller Area Network (CAN). The distributed design closely resembles the communication architecture used in modern vehicles.
The system consists of three independent nodes:
Vision Node (ESP32-CAM)
Headlight Electronic Control Unit (Arduino Uno)
Diagnostic Node (CANHacker Software)
Each node performs a dedicated task while exchanging information through the CAN bus.
The project demonstrates practical implementation of:
Embedded Systems
Automotive CAN Bus Communication
Computer Vision
Automatic Vehicle Lighting
Automotive Diagnostics
Fault-Tolerant Embedded Control
Project Objectives
The objectives of this project were:
Design a distributed CAN-based vehicle headlight control system.
Develop a Vision Node capable of extracting image brightness parameters from camera images.
Implement automatic Day, Dim and Night operating modes.
Detect approaching vehicle headlights using image brightness analysis.
Automatically switch between High Beam and Low Beam.
Implement startup vision self-test.
Implement continuous communication supervision.
Develop CAN-based diagnostic services.
Improve vehicle lighting safety using embedded controllers and CAN communication.
System Architecture
+---------------------+
                    |     12 V Battery    |
                    +----------+----------+
                               |
                               |
                    +----------v----------+
                    | DC-DC Buck Converter|
                    +----------+----------+
                               |
              +----------------+----------------+
              |                                 |
              |                                 |
     +--------v--------+              +---------v--------+
     |  Headlight ECU  |<------------>|   Vision Node    |
     |   Arduino Uno   |    CAN Bus   |    ESP32-CAM     |
     +--------+--------+              +---------+--------+
              |
              |
              |
     +--------v--------+
     | Vehicle Headlamp|
     +-----------------+

              ^
              |
       CANHacker Software
      (Diagnostic Node)
The developed prototype consists of three distributed embedded nodes communicating through the Controller Area Network (CAN).
Plain text
The distributed architecture allows each node to perform its own function independently while exchanging information through the CAN bus.
System Description
1. Vision Node (ESP32-CAM)
The Vision Node is responsible for image acquisition and image brightness analysis.
The ESP32-CAM continuously captures grayscale images and processes a predefined Region of Interest (ROI).
The following parameters are extracted:
brightAvg
Average brightness of the selected ROI.
brightCount
Number of bright pixels within the selected ROI.
These two parameters are periodically transmitted to the Headlight ECU through the CAN bus.
Additional Vision Node Responsibilities
Camera initialization.
Image capture.
ROI image processing.
CAN transmission of image brightness parameters.
Startup handshake acknowledgement.
Continuous communication acknowledgement.
Camera capture health reporting.
Watchdog timer recovery.
Important
The Vision Node does not perform vehicle detection. Vehicle detection is entirely performed by the Headlight ECU using the transmitted image brightness parameters.
2. Headlight Electronic Control Unit (Arduino Uno)
The Headlight ECU is the central controller of the system.
It receives information from:
Ambient Light Sensor (LDR)
Vision Node
The ECU processes:
LDR value
brightAvg
brightCount
Using these inputs, the ECU performs:
Day mode determination.
Dim mode determination.
Night mode determination.
Vehicle detection.
Automatic High Beam control.
Automatic Low Beam control.
Manual Override.
LCD information display.
Startup Vision Node self-test.
Continuous Vision Node supervision.
Communication timeout detection.
Diagnostic services.
Safe-mode operation.
3. Diagnostic Node (CANHacker)
The Diagnostic Node is implemented using CANHacker software.
Its purpose is to provide communication monitoring and diagnostic capabilities during system development and testing.
Functions include:
Monitoring CAN traffic.
Sending diagnostic commands.
Reading ECU information.
Reading Vision Node information.
Displaying image brightness parameters.
Actuator testing.
Diagnostic Trouble Code (DTC) retrieval.
DTC clearing.
Communication verification.
System Operation
The following sequence describes the complete operation of the developed prototype.
1. System Power-Up
When the vehicle ignition switch is turned ON, electrical power is supplied to the Headlight ECU and the Vision Node through the DC-DC buck converter.
The Headlight ECU initializes:
CAN Controller
LCD Display
LDR Sensor
Relay Outputs
Diagnostic Functions
At the same time, the Vision Node initializes:
ESP32-CAM
Camera configuration
CAN Controller
Watchdog Timer
2. Startup Vision Self-Test
Immediately after initialization, the Headlight ECU transmits a CAN handshake request to the Vision Node.
The Vision Node replies with an acknowledgement message indicating that:
Camera initialized successfully
CAN communication is operational
Image acquisition is ready
Test Passed
If acknowledgement is received within the specified timeout period:
Startup self-test passes.
The LCD displays:

Vision Test
Passed
The system enters normal operation.
Test Failed
If acknowledgement is not received:
Startup self-test fails.
The LCD displays:

Vision Test
Failed
The ECU automatically enters Safe Mode, where:
Low Beam remains ON.
Continuous communication monitoring continues.
Normal operation resumes automatically once communication is restored.
3. Ambient Light Detection
The Headlight ECU continuously reads the LDR sensor.
To eliminate unnecessary switching caused by temporary light changes, a confirmation algorithm is implemented before changing operating modes.
The ECU classifies ambient lighting into:
DAY
DIM
NIGHT
Only after the selected condition remains stable for the confirmation period is the operating mode changed.
4. Image Brightness Processing
The Vision Node continuously captures grayscale images.
Only a predefined Region of Interest (ROI) is processed.
For every captured frame, two image brightness parameters are calculated:
brightAvg
brightCount
These values are transmitted periodically through the CAN bus.
No vehicle detection is performed by the Vision Node.
5. Vehicle Detection
Vehicle detection is performed exclusively by the Headlight ECU.
The ECU analyses:
Ambient light condition
brightAvg
brightCount
A state-machine-based detection algorithm is implemented to improve stability.
The algorithm consists of three states:
IDLE
The ECU continuously monitors image brightness while waiting for significant changes.
CANDIDATE
When a sudden increase in bright pixels is observed, the system enters Candidate mode.
Multiple consecutive frames must satisfy the detection criteria before confirming an approaching vehicle.
This prevents false detection caused by:
Reflections
Camera noise
Temporary light disturbances
VEHICLE DETECTED
Once confirmed:
High Beam is switched OFF.
Low Beam is switched ON.
The ECU continuously monitors the transmitted brightness parameters.
When the approaching vehicle disappears for the required confirmation period:
High Beam is restored automatically.
Safety Features
The developed prototype incorporates several safety mechanisms.
Startup Vision Self-Test
Verifies Vision Node availability before allowing normal operation.
Continuous Communication Supervision
The Headlight ECU periodically sends supervision requests to the Vision Node.
Successful acknowledgement confirms:
CAN communication
Camera operation
Camera Health Monitoring
The Vision Node periodically reports image capture status.
Camera capture failure is immediately detected by the ECU.
CAN Communication Timeout Detection
If expected CAN messages are not received within the timeout period:
Communication fault is declared.
Safe Mode is activated.
Safe Low Beam Operation
During communication failure:
High Beam is disabled.
Low Beam remains ON.
This ensures safe vehicle operation even if the Vision Node becomes unavailable.
Manual Override
The driver may manually control High Beam whenever required.
Manual Override remains available even during communication fault conditions.
Software Watchdog Timer
The Vision Node implements a watchdog timer to automatically recover from unexpected software failures.
Diagnostic Trouble Codes (DTC)
Faults detected by the ECU are stored and may be:
Retrieved
Displayed
Cleared
through CANHacker.
CAN Communication
The developed prototype uses the Controller Area Network (CAN) for communication between distributed nodes.
Unlike point-to-point communication, CAN is a broadcast communication protocol, meaning every transmitted message is visible to all connected nodes. Each node decides whether to process or ignore a message according to its programmed CAN Identifier (ID).
Vision Node → Headlight ECU
CAN ID
Data
Description
0x100
brightAvg, brightCount
Image brightness parameters extracted from the selected ROI.
0x207
1
Startup Vision Test acknowledgement.
0x111
ACK
Continuous communication supervision acknowledgement.
0x220
0 or 1
Camera capture status (1 = Successful capture, 0 = Capture failure).
Headlight ECU → Vision Node
CAN ID
Function
0x110
Startup handshake request and continuous communication supervision request.
0x201
System status broadcast.
0x107
Vision Node communication test.
Diagnostic Commands
The Diagnostic Node communicates with the Headlight ECU using the following CAN commands.
CAN ID
Command
Description
0x101
High Beam Test
Forces the ECU to energize the High Beam relay for actuator verification.
0x102
Low Beam Test
Activates the Low Beam relay for testing.
0x103
Lights OFF Test
Switches OFF both relays to verify output operation.
0x104
Read Diagnostic Trouble Codes
Displays all stored ECU faults on the LCD.
0x105
Clear Diagnostic Trouble Codes
Removes all stored faults from ECU memory.
0x106
ECU Information
Displays software version and system runtime information.
0x107
Vision Communication Test
Verifies communication between the Headlight ECU and Vision Node.
0x108
Vision Parameters Display
Displays the current LDR value, brightAvg and brightCount on the LCD for diagnostic purposes.
Hardware Components
Component
Purpose
ESP32-CAM
Vision Node for image acquisition and brightness analysis
Arduino Uno
Headlight Electronic Control Unit
MCP2515 CAN Modules
CAN communication interface
LDR Sensor
Ambient light measurement
Relay Module
Headlight switching
16×2 I²C LCD
Driver Information Display
Manual Override Switch
Manual High Beam control
DC-DC Buck Converter
Converts vehicle battery voltage to regulated supply
LED Vehicle Headlamp
Lighting actuator
12 V Battery
System power source
Software Requirements
Arduino IDE
ESP32 Board Package
MCP_CAN Library
LiquidCrystal_I2C Library
ESP32 Camera Library
CANHacker
Repository Structure

The repository is organized into independent folders to simplify navigation and future development.

CAN-Based-Vision-Assisted-Smart-Vehicle-Headlight-Control-System/
│
├── README.md
├── LICENSE
│
├── Documentation/
│   ├── Final_Project_Report.pdf
│   ├── Presentation_Slides.pdf
│   └── Project_Photos/
│
├── Headlight_ECU/
│   ├── Headlight_ECU.ino
│   └── Libraries_Used.md
│
├── Vision_Node/
│   ├── Vision_Node.ino
│   └── Camera_Settings.md
│
├── Diagnostic_Node/
│   ├── CAN_IDs.md
│   ├── CANHacker_Commands.md
│   └── Testing_Procedure.md
│
├── Hardware/
│   ├── Circuit_Diagram.pdf
│   ├── Wiring_Diagram.pdf
│   ├── Block_Diagram.png
│   ├── Prototype_Assembly/
│   └── Components_List.md
│
├── Images/
│   ├── Prototype/
│   ├── LCD_Display/
│   ├── Testing/
│   └── System_Architecture/
│
└── Videos/
    └── Demonstration_Link.md

---

Installation Guide

Software Installation

Install the following software before uploading the firmware.

- Arduino IDE
- ESP32 Board Package
- MCP_CAN Library
- LiquidCrystal_I2C Library
- ESP32 Camera Library

---

Hardware Setup

1. Assemble the hardware according to the circuit diagram provided in the Hardware folder.
2. Connect both MCP2515 modules through the CAN Bus.
3. Connect the LDR sensor to the Headlight ECU.
4. Connect the relay module to the headlamp output.
5. Connect the LCD to the Arduino using the I²C interface.
6. Supply both nodes from the regulated DC-DC Buck Converter.

---

Firmware Upload

Upload the firmware in the following order:

1. Vision_Node
2. Headlight_ECU

After uploading:

- Connect both CAN modules.
- Power the system.
- Observe the startup self-test on the LCD.

---

Testing Procedure

The following tests were performed during prototype evaluation.

Startup Vision Test

- Power ON the system.
- Verify successful Vision Test on the LCD.

Expected result:

- System enters normal operation.

---

Ambient Light Test

Expose the LDR to:

- Bright light
- Moderate light
- Darkness

Verify transitions between:

- DAY
- DIM
- NIGHT

---

Vehicle Detection Test

During NIGHT mode:

- Direct an external light source toward the camera.

Expected result:

- High Beam switches OFF.
- Low Beam switches ON.

Remove the light source.

Expected result:

- High Beam is automatically restored.

---

Manual Override Test

Operate the Manual Override switch.

Expected result:

- High Beam activates immediately.
- LCD displays Manual Mode.
- Returning the switch restores automatic operation.

---

Communication Fault Test

Disconnect the Vision Node CAN communication.

Expected result:

- Communication timeout detected.
- Safe Low Beam activated.
- LCD displays communication fault.

Reconnect communication.

Expected result:

- System automatically returns to normal operation.

---

Diagnostic Test

Using CANHacker:

- Perform actuator tests.
- Display ECU information.
- Display Vision Node parameters.
- Verify Vision Node communication.
- Retrieve and clear Diagnostic Trouble Codes.

---

Project Highlights

The developed prototype successfully demonstrates:

- Distributed automotive electronic architecture.
- Controller Area Network (CAN) communication.
- Vision-assisted automatic lighting.
- Intelligent headlight control.
- Embedded diagnostics.
- Communication fault tolerance.
- Startup self-test.
- Automatic recovery after communication restoration.
- Manual override capability.
- Software watchdog protection.

---

Future Improvements

The prototype provides a foundation for future intelligent automotive lighting systems.

Possible future improvements include:

- Artificial Intelligence (AI) based vehicle recognition.
- Lane-aware adaptive lighting.
- Rain and fog compensation.
- Steering angle adaptive headlights.
- Vehicle speed integration.
- GPS-assisted lighting control.
- Integration with production automotive ECUs.
- Over-the-air firmware updates.
- Mobile diagnostic application.
- Cloud-based vehicle monitoring.

---

Contributing

Suggestions, improvements and technical discussions are welcome.

If you would like to improve this project:

1. Fork the repository.
2. Create a new branch.
3. Commit your modifications.
4. Submit a Pull Request.

Constructive feedback is highly appreciated.

---

Author

Masoud M. Subi

Bachelor Degree in Automobile Engineering

National Institute of Transport (NIT)

Final Year Project — 2026

---

Acknowledgements

Special appreciation is extended to the National Institute of Transport, the Department of Automobile Engineering, my project supervisor, fellow students and everyone who provided academic, technical and moral support throughout the development of this project.

---

License

© 2026 Masoud M. Subi. All Rights Reserved.

This project was developed for academic and research purposes. The source code, documentation and associated materials may not be reproduced, redistributed or used for commercial purposes without prior written permission from the author.

---

Contact

For questions, technical discussions or collaboration, please open an Issue in this repository or contact the author through the details provided in the project documentation.
