//==========================================================================
// PROJECT : CAN BUS BASED SMART VEHICLE HEADLIGHT CONTROL SYSTEM
//==========================================================================
// Author      : Eng. Masoud M. Subi
// Institution : National Institute of Transport (NIT)
// Year        : 2026
//
// DESCRIPTION
// ------------
// This program implements the CAN Diagnostic Node of a distributed
// CAN-based intelligent vehicle headlight control system. The node
// interfaces with CANHacker V2.00.01 software through a USB serial
// connection, allowing CAN Bus sniffing.
//
// MAIN FUNCTIONS
// --------------
// • Interfaces the CAN network with CANHacker V2.00.01.
// • Shows real-time CAN frames and supports reverse engineering.
// • Sends diagnostic commands to the ECU.
// • Initiates actuator tests.
// • Requests ECU information.
// • Retrieves stored Diagnostic Trouble Code (DTC).
// • Clears stored Diagnostic Trouble Code (DTC).
// • Initiates Vision Node communication verification.
//
// SYSTEM ARCHITECTURE
// -------------------
// CANHacker V2.00.01 ---> Diagnostic Node ---> CAN Bus ---> Headlight ECU
//
//==========================================================================


#include <can.h>
#include <mcp2515.h>

#include <CanHacker.h>
#include <CanHackerLineReader.h>
#include <lib.h>

#include <SPI.h>
#include <SoftwareSerial.h>

const int SPI_CS_PIN = 10;
const int INT_PIN = 9;

const int SS_RX_PIN = 3;
const int SS_TX_PIN = 4;

CanHackerLineReader *lineReader = NULL;
CanHacker *canHacker = NULL;

SoftwareSerial softwareSerial(SS_RX_PIN, SS_TX_PIN);

void handleError(const CanHacker::ERROR error);

void setup() {
    Serial.begin(115200);
    SPI.begin();
    softwareSerial.begin(115200);

    Stream *interfaceStream = &Serial;
    Stream *debugStream = &softwareSerial;
    
    
    canHacker = new CanHacker(interfaceStream, debugStream, SPI_CS_PIN);
    canHacker->setClock(MCP_8MHZ);    
    //canHacker->enableLoopback(); // remove to disable loopback test mode
    lineReader = new CanHackerLineReader(canHacker);
    
    pinMode(INT_PIN, INPUT);
}

void loop() {
    CanHacker::ERROR error;
    
    if (digitalRead(INT_PIN) == LOW) {
        error = canHacker->processInterrupt();
        handleError(error);
    }
    
    // uncomment that lines for Leonardo, Pro Micro or Esplora
    // error = lineReader->process();
    // handleError(error);
}

// serialEvent handler not supported by Leonardo, Pro Micro and Esplora
void serialEvent() {
    CanHacker::ERROR error = lineReader->process();
    handleError(error);
}

void handleError(const CanHacker::ERROR error) {

    switch (error) {
        case CanHacker::ERROR_OK:
        case CanHacker::ERROR_UNKNOWN_COMMAND:
        case CanHacker::ERROR_NOT_CONNECTED:
        case CanHacker::ERROR_MCP2515_ERRIF:
        case CanHacker::ERROR_INVALID_COMMAND:
            return;

        default:
            break;
    }
  
    softwareSerial.print("Failure (code ");
    softwareSerial.print((int)error);
    softwareSerial.println(")");
  
    while (1) {
        delay(2000);
    } ;
}
