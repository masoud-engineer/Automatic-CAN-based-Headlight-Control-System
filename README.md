Smart Vehicle Headlight Control System Using ESP32-CAM and CAN Bus

Overview

This project presents a CAN-based Smart Vehicle Headlight Control System developed to improve driving safety by automatically controlling vehicle headlights according to ambient lighting conditions while preventing glare to oncoming drivers during night driving.

The system uses an LDR sensor to determine ambient lighting conditions (Day, Dim and Night). During night operation only, an ESP32-CAM continuously analyses a selected region of the road scene and computes two image brightness parameters (BrightAvg and BrightCount). These parameters are transmitted over the CAN Bus to the Arduino-based Electronic Control Unit (ECU), where the vehicle detection algorithm is executed.

Unlike conventional vision systems, the ESP32-CAM does not decide whether a vehicle is present. It only acts as a vision sensing node that provides brightness information. All headlight control decisions are performed by the Arduino ECU.

---

System Architecture

The system consists of three CAN Bus nodes.

1. ESP32-CAM Vision Node

Functions:

- Captures grayscale images.
- Analyses predefined Region of Interest (ROI).
- Calculates:
  - BrightAvg
  - BrightCount
- Transmits brightness data through the CAN Bus.
- Responds to camera communication test requests.

The ESP32-CAM performs image acquisition and brightness measurement only.

---

2. Arduino Headlight Control ECU

Functions:

- Reads ambient light using the LDR sensor.
- Determines:
  - Day Mode
  - Dim Mode
  - Night Mode
- Receives BrightAvg and BrightCount from the ESP32-CAM.
- Filters received brightness values.
- Calculates brightness variation (Delta).
- Executes the vehicle detection algorithm.
- Controls High Beam and Low Beam headlights.
- Performs system diagnostics.
- Stores and clears Diagnostic Trouble Code (DTC).
- Displays system information on the LCD.

The Arduino serves as the main Electronic Control Unit (ECU) responsible for all decision-making.

---

3. CAN Diagnostic Node

Functions:

- Sends CAN diagnostic commands.
- Performs actuator tests.
- Retrieves Diagnostic Trouble Code.
- Clears stored Diagnostic Trouble Code.
- Requests ECU information.
- Initiates camera communication tests.
- Allows evaluation of the ECU without modifying the control software.

---

Operating Principle

1. The LDR continuously measures ambient light intensity.
2. The Arduino determines whether the environment is Day, Dim or Night.
3. During Day:
   - Headlights remain OFF.
4. During Dim:
   - Low Beam is activated.
5. During Night:
   - High Beam is activated.
   - The ESP32-CAM continuously sends BrightAvg and BrightCount values.
   - The Arduino processes these values to determine whether an approaching vehicle is present.
   - If a vehicle is detected:
     - High Beam switches to Low Beam.
   - When the vehicle is no longer detected:
     - High Beam is restored automatically.

---

Main Features

- Automatic Day, Dim and Night headlight control
- Automatic High Beam / Low Beam switching
- Oncoming vehicle detection during night driving
- CAN Bus communication between all nodes
- Manual High Beam override
- CAN communication fault detection
- Fail-safe Low Beam operation
- ECU startup self-test
- Camera communication verification
- High Beam actuator test
- Low Beam actuator test
- Lights OFF actuator test
- Diagnostic Trouble Code (DTC) retrieval
- Diagnostic Trouble Code (DTC) clearing
- ECU information display
- LCD-based Human Machine Interface (HMI)

---

Hardware Components

- ESP32-CAM (AI Thinker)
- Arduino Uno
- MCP2515 CAN Bus Modules
- 16×2 I2C LCD
- LDR Sensor
- Relay Module
- LED Vehicle Headlights
- DC Buck Converter
- 12 V Vehicle Battery

---

Software Tools

- Arduino IDE
- ESP32 Board Package
- MCP_CAN Library
- arduino-canhacker Library
- LiquidCrystal_I2C Library
- CANHacker V2.00.01
- GitHub

---

CANHacker V2.00.01

CANHacker V2.00.01 used together with the CAN Diagnostic Node to:

- Monitor CAN Bus communication.
- Transmit CAN diagnostic commands.
- Perform actuator tests.
- Retrieve Diagnostic Trouble Codes.
- Clear Diagnostic Trouble Codes.
- Display ECU information.
- Verify camera communication.

---

CAN Diagnostic Commands

CAN ID| Function
0x101| High Beam Actuator Test
0x102| Low Beam Actuator Test
0x103| Lights OFF Actuator Test
0x104| Retrieve DTCs
0x105| Clear DTCs
0x106| Display ECU Information
0x107| Camera Communication Test
0x201| ECU Status Broadcast
0x207| Camera Acknowledgement

---

Repository Contents

- Arduino Headlight Control ECU Software
- ESP32-CAM Vision Node Software
- CAN Diagnostic Node Software
- Circuit Diagrams
- Project Documentation
- Images
- Demonstration Material

---

Applications

- Intelligent Automotive Lighting Systems
- Automatic Headlight Control
- CAN Bus Embedded Systems
- Automotive Electronics
- Educational and Research Projects

---

Future Improvements

- Adaptive threshold calibration
- Rain and fog compensation
- Steering angle adaptive lighting
- Vehicle speed integration
- Persistent DTC storage using EEPROM

---

Author

Masoud M. Subi

Bachelor's degree in Automobile Engineering

Project Title:

Smart Vehicle Headlight Control System Using ESP32-CAM and CAN Bus
