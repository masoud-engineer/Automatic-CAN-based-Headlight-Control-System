//==========================================================================
// PROJECT : CAN BUS BASED SMART VEHICLE HEADLIGHT CONTROL SYSTEM
//==========================================================================
// Author      : Eng. Masoud M. Subi
// Institution : National Institute of Transport (NIT)
// Year        : 2026
//
// DESCRIPTION
// ------------
// This program implements the Headlight Control Electronic Control Unit
// (ECU) of a distributed CAN-based intelligent vehicle headlight control
// system. The ECU communicates with an ESP32-CAM Vision Node and a
// CAN Diagnostic Node through the Controller Area Network (CAN).
//
// MAIN FUNCTIONS
// --------------
// • Determines Day, Dim and Night modes using an LDR sensor.
// • Receives BrightAvg and BrightCount from the Vision Node.
// • Detects approaching vehicle headlights during Night operation.
// • Automatically controls headlight beam.
// • Supports manual override operation.
// • Performs CAN communication monitoring and fail-safe protection.
// • Supports actuator testing and ECU information display.
// • Stores, retrieves and clears Diagnostic Trouble Code (DTC).
// • Displays real-time system status on a 16×2 I2C LCD.
//
// SYSTEM ARCHITECTURE
// -------------------
// Vision Node (ESP32-CAM)  --->  CAN Bus  --->  Headlight Control ECU
// Diagnostic Node          --->  CAN Bus  --->  Headlight Control ECU
//
// NOTE
// ----
// The Vision Node performs image brightness analysis only. Vehicle
// detection and all headlight control decisions are executed by this ECU.
//
//==========================================================================


#include <avr/wdt.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <mcp_can.h>


MCP_CAN CAN0(10);      // MCP2515 CS pin
LiquidCrystal_I2C lcd(0x27,16,2);

byte faultRegister = 0;
unsigned long systemStartTime;
bool cameraTest = false;

bool canTimeout = false;
bool manualOverride = false;

unsigned long statusTimer = 0;


bool actuatorTest = false;
byte actuatorMode = 0;
unsigned long actuatorStart = 0;


#define LED_HIGH 4
#define LED_LOW 3
#define OVERRIDE_PIN 2

#define PING_ID 0x110
#define ACK_ID 0x111

bool visionOK = false;
unsigned long lastPingTime = 0;
unsigned long ackWaitStart = 0;
bool waitingACK = false;


#define LDR_PIN A3
int ldr = 0;


unsigned long lastMsgTime = 0;
const unsigned long CAN_TIMEOUT = 2000;



const int LDR_DAY = 210; //(To be calibrated based on real world ambient light condition)

const int LDR_NIGHT = 90; //(To be calibrated based on real world ambient light condition)




const int BRIGHTCOUNT_ON = 60; // (To be tuned based on vehicle detection sensitivity)
const int BRIGHTCOUNT_OFF = 30; // (To be tuned based on vehicle detection sensitivity)
const int BRIGHTAVG_MIN = 60;  // (To be tuned based on vehicle detection sensitivity)


float brightAvg = 0;
float brightCount = 0;


bool vehicleDetected = false;
int previousBrightCount = 0;
const int DELTA_THRESHOLD = 20;
byte vehicleState = 0; // 0 = IDLE, 1 = CANDIDATE, 2 = VEHICLE
byte belowCounter = 0;
byte candidateCounter = 0;


enum LightState
{
    DAY,
    DIM,
    NIGHT
};

LightState lightState = DIM;

byte dayCounter = 0;
byte nightCounter = 0;
byte dimCounter = 0;
#define LDR_CONFIRM_COUNT 60


void visionStartupTest();
void visionMonitor();
void processCANMessages();
void updateSystemLCD();
void headlightControl();
void manualOverrideMode();
void lowBeamOnly();
void updateLCD(String line1, String line2);
void checkVisionACK();
void actuatorTestMode();
void systemStatus();
void detectVehicle();
void sendVisionPing();
void readLDR();


void setup()
{
    wdt_disable();
    Serial.begin(115200);
    
    lcd.init();
    lcd.backlight();

    updateLCD("SMART HEADLIGHT","INITIALIZING...");
    
    delay(1000);
    updateLCD("SELF TEST","PLEASE WAIT");
    pinMode(LED_HIGH, OUTPUT);
    pinMode(LED_LOW, OUTPUT);
    pinMode(OVERRIDE_PIN, INPUT_PULLUP);

    digitalWrite(LED_HIGH, HIGH);
    delay(500);
    digitalWrite(LED_LOW, HIGH);
    digitalWrite(LED_HIGH, LOW);
    delay(500);
    digitalWrite(LED_LOW, LOW);
    digitalWrite(LED_HIGH, LOW);
    
    updateLCD("HEADLIGHT TEST","PASS");
    delay(500);
    
    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    {
        Serial.println("CAN OK");
    }
    else
    {
        while (1);
    }

    CAN0.setMode(MCP_NORMAL);
    
    visionStartupTest();
    
    systemStartTime = millis();
    wdt_enable(WDTO_8S);
}

void updateSystemLCD()
{
    
    if(cameraTest)
    {
      return;  
    }

    if(manualOverride)
    {
       updateLCD("MANUAL MODE","HIGH BEAM");
       return;
    }

  
    if(actuatorTest)
    {
        switch(actuatorMode)
        {
            case 1:
                updateLCD("TEST MODE","HIGH BEAM");
                break;

            case 2:
                updateLCD("TEST MODE","LOW BEAM");
                break;

            case 3:
                updateLCD("TEST MODE","LIGHTS OFF");
                break;
        }

        return;
    }

    if(canTimeout)
    {
        updateLCD("CAN FAULT","LOW BEAM-SAFE");
        return;
    }


    switch(lightState)
    {
        case DAY:

            updateLCD("DAY","HEADLIGHT OFF");

            break;

        case DIM:

            updateLCD("DIM LIGHT","LOW BEAM");

            break;

        case NIGHT:

            if(vehicleDetected)
            {
             updateLCD("VEHICLE AHEAD","LOW BEAM");
            }
            else
            {
              updateLCD("NIGHT","HIGH BEAM");
            }
            break;
    }
}

void updateLCD(String line1, String line2)
{
  static String old1 = "";
  static String old2 = "";
  if( line1 != old1 || line2 != old2)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);


    lcd.setCursor(0,1);
    lcd.print(line2);

    old1 = line1;
    old2 = line2;
  }
}

void loop()
{
    processCANMessages();     // Receive and process all CAN frames
    
    visionMonitor();          // Vision node handshake & CAN health

    readLDR();                // Read LDR

    detectVehicle();          // Vehicle detection state machine

    headlightControl();       // Decide OFF / HIGH / LOW

    actuatorTestMode();       // Perform actuator test

    manualOverrideMode();     // Allow manual override

    systemStatus();           // Send system status

    updateSystemLCD();        // Refresh LCD only when needed

    wdt_reset();              // Feed watchdog

    delay(50);                // Main loop period
    
}

void processCANMessages()
{
    unsigned long id;
    unsigned char len = 0;
    uint8_t msg[8];

    if (CAN0.checkReceive() == CAN_MSGAVAIL)
    {
        CAN0.readMsgBuf(&id,&len,msg);

        if(id == 0x101)
        {
          actuatorTest = true;
          actuatorMode = 1;
          actuatorStart = millis();
        }
        else if (id == 0x102)
        {
          actuatorTest = true;
          actuatorMode = 2;
          actuatorStart = millis(); 
        }
        else if(id == 0x103)
        {
          actuatorTest = true;
          actuatorMode = 3;
          actuatorStart = millis(); 
        }
        else if(id == 0x104)
        {
          updateLCD("RETRIEVING...","PLEASE WAIT");

          delay(2000);

          switch(faultRegister)
          {
           case 0:

           updateLCD("NO FAULTS","SYSTEM OK");
           delay(2000);

           break;

           case 1:

           updateLCD("CAN FAULT","CHECK BUS&CAMERA");
           delay(2000);

           break;

        }
        updateSystemLCD();
        return;
       }
     else if(id==0x105)
       {
         updateLCD("CLEARING","FAULTS");

         delay(2000);
      
         faultRegister = 0;

         updateLCD("NO FAULTS","SYSTEM OK");
         
         delay(2000);
         updateSystemLCD();
         return;
       }
     
     else if (id == 0x106)
      {
        unsigned long minutes = (millis()- systemStartTime)/60000;

        updateLCD("SYSTEM INFO","Run:"+String(minutes)+" Min");
        delay(4000);
        updateSystemLCD();
        return;
      }
     else if(id == 0x107)
      {
        if(canTimeout)
         {
           updateLCD("CAN BUS FAULT","CAM TEST BLOCKED");
           delay(2000);
           updateSystemLCD();
           return;
         }
        else
         {
           byte msg[1] = {1};
           cameraTest = true;
      
           CAN0.sendMsgBuf(0x107, 0, 1, msg);
      
           updateLCD("CHECKING","CAMERA");
      
           delay(2000);
           return;
         }
      }
     
     else if(id == 0x207)
      {
        updateLCD("CAMERA","OK");
      
        delay(2000);
        cameraTest = false;
        wdt_reset();
        updateSystemLCD();
        return;
      }
     
     else if(id == 0x108)
      {
        updateLCD("LDR:"+String(ldr),"A:"+String(brightAvg)+" C:"+String(brightCount));

        delay(4000);
      
        updateSystemLCD();
        return;
      }

     else if(id == ACK_ID)
      {

        if(msg[0]=='A' && msg[1]=='C' && msg[2]=='K')
            {
                visionOK=true;

                waitingACK = false;
                canTimeout = false;

            }

      }
     else if(id == 0x100)
      {
        uint8_t brightAvgRaw   = msg[0];
        uint8_t brightCountRaw = msg[1];

        brightAvg   = 0.85 * brightAvg   + 0.15 * brightAvgRaw;
        brightCount = 0.85 * brightCount + 0.15 * brightCountRaw;
        Serial.print("Bright Avg : ");
        Serial.print(brightAvg);

        Serial.print("   Bright Count : ");
        Serial.println(brightCount);
      }
   }
}

void detectVehicle()
{

int delta = brightCount - previousBrightCount;
previousBrightCount = brightCount;

Serial.print("Delta : ");
Serial.println(delta);

switch(vehicleState)
{
case 0:     // IDLE

    if(delta > DELTA_THRESHOLD)
    {
        vehicleState = 1;
    }

    break;


case 1:     // CANDIDATE

    if(brightCount > BRIGHTCOUNT_ON &&
       brightAvg > BRIGHTAVG_MIN)
    {
        candidateCounter++;
       if(candidateCounter >= 3)
       {
        vehicleDetected = true;
        vehicleState = 2;
        belowCounter = 0;
        candidateCounter = 0;
       }
    }
    else 
    {
     candidateCounter = 0; 
    if(brightCount < BRIGHTCOUNT_ON)
    {
        vehicleState = 0;
    }
    }
    break;


case 2:     // VEHICLE DETECTED

    if(brightCount < BRIGHTCOUNT_OFF)
    {
        belowCounter++;

        if(belowCounter >= 10)      // about 0.5 s (10 × 50 ms)
        {
            vehicleDetected = false;
            vehicleState = 0;
            belowCounter = 0;
        }
    }
    else
    {
        belowCounter = 0;
    }

    break;
}
}

void readLDR()
{
 
ldr = analogRead(LDR_PIN); //-------READ LDR------------

Serial.print("LDR : ");
Serial.println(ldr);

if(ldr > LDR_DAY)
{
    dayCounter++;
    nightCounter = 0;

    if(dayCounter >= LDR_CONFIRM_COUNT)
    {
        lightState = DAY;
        dayCounter = LDR_CONFIRM_COUNT;
    }
}
else if(ldr < LDR_NIGHT)
{
    nightCounter++;
    dayCounter = 0;

    if(nightCounter >= LDR_CONFIRM_COUNT)
    {
        lightState = NIGHT;
        nightCounter = LDR_CONFIRM_COUNT;
    }
}
else
{
    dimCounter++;
    dayCounter = 0;
    nightCounter = 0;

   if(dimCounter >= LDR_CONFIRM_COUNT)
   {
    lightState = DIM;
    dimCounter = LDR_CONFIRM_COUNT;
   }
}
}

void actuatorTestMode()
{
  
if(actuatorTest)
{
  switch(actuatorMode)
  {
    case 1:
        digitalWrite(LED_HIGH,HIGH);
        digitalWrite(LED_LOW,LOW);

        break;

     case 2:
        lowBeamOnly();

        break;

     case 3:
        digitalWrite(LED_HIGH,LOW);
        digitalWrite(LED_LOW,LOW);

        break;
  }
  if(millis()-actuatorStart >= 3000)
  {
    actuatorTest = false;
  }
  return;
}
}

void headlightControl()
{
  
  if(manualOverride)
  {
    digitalWrite(LED_HIGH,HIGH);
    digitalWrite(LED_LOW,LOW);
    return;
  }
  if(canTimeout)
  {
  lowBeamOnly();
  return;
  }
  
switch(lightState)
{

case DAY:

    digitalWrite(LED_HIGH,LOW);
    digitalWrite(LED_LOW,LOW);
    break;
case DIM:

    digitalWrite(LED_HIGH,LOW);
    digitalWrite(LED_LOW,HIGH);
    break;
case NIGHT:
    if(vehicleDetected)
    {
        lowBeamOnly();
    }
    else
    {
            digitalWrite(LED_HIGH,HIGH);
            digitalWrite(LED_LOW,LOW);
    }
    break;
}
}

void manualOverrideMode()
{

if(digitalRead(OVERRIDE_PIN)==LOW)
{
    digitalWrite(LED_HIGH,HIGH);
    manualOverride = true;
}
else
{
  manualOverride = false;
}
}

void systemStatus()
{
  if(millis()-statusTimer >=1000)
{
  statusTimer = millis();
  byte status[5];
  status[0] = lightState;
  status[1] = vehicleDetected;
  status[2] = actuatorTest;
  status[3] = digitalRead(OVERRIDE_PIN)==LOW;
  status[4] = 0;

  CAN0.sendMsgBuf(0x201,0,5,status);
}
}

void sendVisionPing()
{
 byte pingData[8];
  pingData[0] = 'P';
  pingData[1] = 'I';
  pingData[2] = 'N';
  pingData[3] = 'G';

  CAN0.sendMsgBuf(PING_ID, 0, 4, pingData); 
}

void checkVisionACK()
{

    if(CAN_MSGAVAIL == CAN0.checkReceive())
    {

        long unsigned int rxId;

        byte len;

        byte rxBuf[8];


        CAN0.readMsgBuf(&rxId,&len,rxBuf);


        if(rxId == ACK_ID)
        {

            if(rxBuf[0]=='A' &&
               rxBuf[1]=='C' &&
               rxBuf[2]=='K')
            {

                visionOK=true;

                waitingACK = false;
                canTimeout = false;

            }

        }

    }

}

void visionMonitor()
{

    unsigned long now = millis();


    // send ping every second

    if(now-lastPingTime >=1000)
    {

        sendVisionPing();

        lastPingTime=now;


        waitingACK=true;

        ackWaitStart=now;

    }

    if(waitingACK &&
       now-ackWaitStart >500)
    {

        visionOK=false;
        waitingACK=false;

        canTimeout = true;
        faultRegister |= 0x01;

    }
}

void visionStartupTest()
{
    updateLCD("VISION TEST","CHECKING");

    while(!visionOK)
    {
        sendVisionPing();

        unsigned long start = millis();

        while(millis() - start < 500)
        {
            checkVisionACK();

            if(visionOK)
                break;
        }

        if(!visionOK)
        {
            canTimeout = true;
            faultRegister |= 0x01;
           
            manualOverrideMode();

            if(manualOverride)
            {
              updateLCD("MANUAL MODE","HIGH BEAM");
            }
            else
            {
              lowBeamOnly();
              updateLCD("VISION FAIL","LOW BEAM SAFE");
            }
            delay(1000);
        }
   }

    updateLCD("VISION TEST","PASS");
    delay(1000);

    updateLCD("SYSTEM","READY");
    delay(1000);

    lastPingTime = millis();
    waitingACK = false;
}

void lowBeamOnly()
{
  digitalWrite(LED_HIGH,LOW);
  digitalWrite(LED_LOW,HIGH);
}
