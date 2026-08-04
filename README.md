# CAN-Based Vision-Assisted Smart Vehicle Headlight Control System

## Overview

The **CAN-Based Vision-Assisted Smart Vehicle Headlight Control System** is a distributed embedded automotive lighting prototype developed as a Bachelor Degree in Automobile Engineering final-year project.

The project demonstrates the implementation of an intelligent vehicle lighting system using **Controller Area Network (CAN)** communication, embedded control and computer vision. The system automatically controls vehicle headlight operation according to ambient lighting conditions and approaching vehicle headlights while providing diagnostic capabilities and communication fault management.

Unlike conventional academic prototypes that combine sensing and control within a single controller, this project adopts a **distributed Electronic Control Unit (ECU)** architecture, closely resembling the design philosophy used in modern vehicles.

---

# Project Objectives

The project was developed to:

- Design a distributed CAN-based vehicle headlight control system.
- Develop a Vision Node capable of extracting image brightness parameters from camera images.
- Implement automatic Day, Dim and Night headlight operation.
- Detect approaching vehicle headlights using image brightness analysis.
- Automatically switch between High Beam and Low Beam.
- Implement startup self-test and continuous communication supervision.
- Develop CAN-based diagnostic functions.
- Improve vehicle lighting safety using low-cost embedded hardware.

---

# System Architecture

The system consists of three independent nodes connected through the **Controller Area Network (CAN)**.

- Vision Node (ESP32-CAM)
- Headlight Electronic Control Unit (Arduino Uno)
- Diagnostic Node (CANHacker)

Each node performs an independent function while exchanging information over the CAN bus.

---

# System Description

## Vision Node (ESP32-CAM)

The Vision Node continuously captures grayscale images using the ESP32-CAM module.

A predefined **Region of Interest (ROI)** is processed to compute:

- **brightAvg** – Average image brightness.
- **brightCount** – Number of bright pixels.

Only these two parameters are transmitted through the CAN bus.

> **Important**
>
> The Vision Node **does not perform vehicle detection**. It only extracts image brightness information.

### Additional Responsibilities

- Startup acknowledgement
- Continuous communication acknowledgement
- Camera health monitoring
- Watchdog recovery

---

## Headlight Electronic Control Unit (Arduino Uno)

The Headlight ECU is the central decision-making controller.

It receives:

- LDR sensor value
- brightAvg
- brightCount

The ECU performs:

- Ambient light classification
- Vehicle detection
- Automatic headlight switching
- Startup Vision Node self-test
- Continuous Vision Node supervision
- CAN communication timeout detection
- Manual override
- Diagnostic processing
- Safe-mode operation
- LCD information display

---

## Diagnostic Node

The Diagnostic Node is implemented using **CANHacker** software.

Its purpose is to:

- Send diagnostic requests
- Monitor CAN traffic
- Verify Vision Node communication
- Perform actuator testing
- Retrieve Diagnostic Trouble Codes (DTCs)
- Clear Diagnostic Trouble Codes (DTCs)

---

# Operating Principle

1. Vehicle ignition is switched ON.
2. The Headlight ECU performs startup initialization.
3. A CAN handshake request is transmitted to the Vision Node.
4. The Vision Node acknowledges successful communication.
5. Normal operation begins.
6. The Vision Node continuously transmits:
   - **brightAvg**
   - **brightCount**
7. The Headlight ECU reads the ambient light level from the LDR sensor.
8. After confirmation timing, the ECU determines whether the environment is:
   - Day
   - Dim
   - Night
9. During confirmed Night operation, the ECU analyses **brightAvg** and **brightCount** using the implemented vehicle detection state machine.
10. If an approaching vehicle is detected:
    - High Beam switches OFF.
    - Low Beam switches ON.
11. After the approaching vehicle disappears and confirmation conditions are satisfied:
    - High Beam is restored automatically.
12. Throughout system operation, communication supervision continues in the background.

---

# Safety Features

The system incorporates several safety mechanisms:

- Startup Vision Node self-test
- Continuous communication supervision
- Camera health monitoring
- CAN timeout detection
- Automatic Safe Low Beam mode
- Manual override
- Software Watchdog Timer
- Diagnostic Trouble Code (DTC) support

These features improve overall system reliability and fault tolerance.

---

# CAN Communication

> **Note:** The CAN communication protocol is a **broadcast-type message transfer protocol**, where every node receives transmitted messages. Each node is programmed to process only the CAN IDs relevant to its operation.

## Vision Node → Headlight ECU

| CAN ID | Data | Description |
|--------:|------|-------------|
| **0x100** | brightAvg, brightCount | Image brightness parameters |
| **0x207** | 1 | Vision Node test acknowledgement |
| **0x111** | ACK | Vision Node continuous monitoring acknowledgement |
| **0x220** | 0 or 1 | Camera capture status (1 = Successful, 0 = Failed) |

---

## Headlight ECU → Vision Node

| CAN ID | Function |
|--------:|----------|
| **0x110** | Startup handshake request and continuous communication supervision request |
| **0x201** | System status |
| **0x107** | Camera (Vision Node) communication test |

---

# Diagnostic Commands

| CAN ID | Command | Function |
|--------:|---------|----------|
| **0x101** | High Beam Test | Activates High Beam relay for actuator verification |
| **0x102** | Low Beam Test | Activates Low Beam relay for actuator verification |
| **0x103** | Lights OFF Test | Turns OFF both relays for output verification |
| **0x104** | Read Diagnostic Trouble Codes | Displays stored ECU faults |
| **0x105** | Clear Diagnostic Trouble Codes | Clears stored faults from memory |
| **0x106** | ECU Information | Displays system run-time information |
| **0x107** | Vision Communication Test | Verifies Vision Node communication |
| **0x108** | Vision Parameters Display | Displays LDR value, brightAvg and brightCount on the LCD |

---

# Hardware Components

- ESP32-CAM (AI Thinker)
- Arduino Uno
- MCP2515 CAN Bus Modules
- LDR Sensor
- 16×2 I²C LCD
- Relay Module
- LED Vehicle Headlamp
- Manual Override Switch
- DC-DC Buck Converter
- 12 V Power Supply

---

# Software

- Arduino IDE
- ESP32 Board Package
- MCP_CAN Library
- LiquidCrystal_I2C Library
- CANHacker

---

# Future Improvements

Potential future enhancements include:

- Artificial Intelligence (AI) based vehicle recognition
- Adaptive threshold calibration
- Rain and fog compensation
- Steering angle adaptive lighting
- Vehicle speed integration
- Integration with production automotive CAN networks
- Dedicated PC diagnostic application

---
## Documentation

The complete project report is available in the **Releases** section of this repository.

**Latest Release:**  
https://github.com/USERNAME/REPOSITORY/releases/latest

# Author

**Masoud M. Subi**

Bachelor Degree in Automobile Engineering

National Institute of Transport (NIT)

2026

---

# License

This repository has been developed for academic and research purposes.

Permission should be obtained from the author before reproduction, redistribution or commercial application of any part of this work.
