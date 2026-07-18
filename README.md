# Smart Vehicle Headlight Control System Using CAN Bus

## Overview

This project presents a **CAN-based Smart Vehicle Headlight Control System** developed to improve driving safety by automatically controlling vehicle headlights according to ambient lighting conditions while preventing glare to oncoming drivers during night driving.

The system uses an **LDR sensor** to determine ambient lighting conditions (**Day, Dim, and Night**). An **ESP32-CAM** continuously analyses a selected region of the road scene and computes two image brightness parameters:

- **BrightAvg** – Average brightness value of the selected Region of Interest (ROI)
- **BrightCount** – Number of detected bright pixels in the ROI

These parameters are transmitted over the **CAN bus** to the Arduino-based **Electronic Control Unit (ECU)**, where the vehicle detection algorithm is executed during night operation only.

---

# System Architecture

The system consists of three CAN bus nodes:

## 1. CAM Vision Node (ESP32-CAM)

### Functions:
- Captures grayscale images.
- Analyses predefined Region of Interest (ROI).
- Calculates image brightness parameters:
  - Average brightness
  - Bright pixel count
- Transmits brightness data through the CAN bus.
- Responds to camera communication test requests.

The ESP32-CAM performs **image acquisition and brightness measurement only**. All decision-making is performed by the Arduino ECU.

---

## 2. Arduino Headlight Control ECU

### Functions:
- Reads ambient light intensity using the LDR sensor.
- Determines operating mode:
  - Day mode
  - Dim mode
  - Night mode
- Receives BrightAvg and BrightCount values from the ESP32-CAM.
- Filters received brightness data.
- Calculates brightness variation (Delta).
- Executes the vehicle detection algorithm.
- Controls high beam and low beam operation.
- Performs system diagnostics.
- Stores and clears Diagnostic Trouble Codes (DTCs).
- Displays system information through the LCD.

The Arduino acts as the main **Electronic Control Unit (ECU)** responsible for system decision-making.

---

## 3. CAN Diagnostic Node

### Functions:
- Sends CAN diagnostic commands.
- Performs headlight actuator tests.
- Retrieves Diagnostic Trouble Codes.
- Clears stored Diagnostic Trouble Codes.
- Requests ECU information.
- Initiates camera communication tests.

---

# Operating Principle

The LDR continuously measures ambient lighting intensity.

The Arduino ECU determines the current lighting condition:

### Day Mode
- Headlights remain OFF.

### Dim Mode
- Low beam headlights are activated.

### Night Mode
- High beam headlights are activated.
- ESP32-CAM brightness data is processed to detect approaching vehicles.

When an approaching vehicle is detected:

- High beam is switched OFF.
- Low beam is activated to prevent glare.

When the vehicle is no longer detected:

- High beam operation is automatically restored.

---

# Main Features

- Automatic day, dim, and night headlight control.
- Automatic high beam and low beam switching.
- Oncoming vehicle detection during night driving.
- CAN bus communication between all system nodes.
- Manual high beam override.
- CAN communication fault detection.
- Fail-safe low beam operation.
- ECU startup self-test.
- Camera communication verification.
- High beam actuator test.
- Low beam actuator test.
- Lights OFF actuator test.
- Diagnostic Trouble Code (DTC) retrieval.
- Diagnostic Trouble Code (DTC) clearing.
- ECU information display.
- LCD-based Human Machine Interface (HMI).

---

# Hardware Components

- ESP32-CAM (AI Thinker)
- Arduino Uno
- MCP2515 CAN Bus Modules
- 16×2 I2C LCD
- LDR Sensor
- Relay Module
- LED Vehicle Headlight
- DC Buck Converter
- 12V Battery

---

# Software Tools

- Arduino IDE
- ESP32 Board Package
- MCP_CAN Library
- arduino-canhacker Library
- LiquidCrystal_I2C Library
- CANHacker V2.00.01
- GitHub

---

# CANHacker V2.00.01

CANHacker V2.00.01 is used together with the CAN Diagnostic Node to:

- Monitor CAN bus communication.
- Transmit diagnostic commands.
- Perform actuator tests.
- Retrieve Diagnostic Trouble Codes.
- Clear Diagnostic Trouble Codes.
- Display ECU information.
- Verify camera communication.

---

# CAN Diagnostic Commands

| CAN ID | Function |
|--------|----------|
| 0x101 | High beam actuator test |
| 0x102 | Low beam actuator test |
| 0x103 | Lights OFF actuator test |
| 0x104 | Retrieve DTCs |
| 0x105 | Clear DTCs |
| 0x106 | Display ECU information |
| 0x107 | Camera communication test |
| 0x201 | ECU status broadcast |
| 0x207 | Camera acknowledgement |

---

# Repository Contents

This repository contains:

- Arduino Headlight Control ECU software
- ESP32-CAM Vision Node software
- CAN Diagnostic Node software
- Circuit diagrams
- Project documentation
- Images
- Demonstration materials

---

# Applications

- Intelligent automotive lighting systems
- Automatic headlight control systems
- CAN bus embedded systems
- Automotive electronics
- Educational and research projects

---

# Future Improvements

Possible future developments include:

- Adaptive threshold calibration.
- Rain and fog compensation.
- Steering angle adaptive lighting.
- Vehicle speed integration.
- Persistent DTC storage using EEPROM.

---

# Author

**Masoud M. Subi**  
Bachelor's Degree in Automobile Engineering  

**Project Title:**  
Smart Vehicle Headlight Control System Using CAN Bus
