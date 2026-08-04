//==========================================================================
// PROJECT : CAN BUS BASED SMART VEHICLE HEADLIGHT CONTROL SYSTEM
//==========================================================================
// Author      : Eng. Masoud M. Subi
// Institution : National Institute of Transport (NIT)
// Year        : 2026
//
// DESCRIPTION
// ------------
// This program implements the Vision Node of a distributed CAN-based
// intelligent vehicle headlight control system. The ESP32-CAM captures
// grayscale images, analyses predefined Region of Interest (ROI), and
// periodically transmits image brightness parameters to the Headlight
// Control ECU through the Controller Area Network (CAN).
//
// MAIN FUNCTIONS
// --------------
// • Captures grayscale images using the ESP32-CAM.
// • Analyses predefined image Region of Interest (ROI).
// • Computes BrightAvg (average brightness).
// • Computes BrightCount (number of bright pixels).
// • Periodically transmits BrightAvg and BrightCount via CAN Bus.
// • Responds to ECU communication test requests.
//
// SYSTEM ARCHITECTURE
// -------------------
// ESP32-CAM Vision Node ---> CAN Bus ---> Headlight Control ECU
//
// NOTE
// ----
// This node performs image acquisition and brightness analysis only.
// Vehicle detection and headlight control decisions are performed by
// the Headlight Control ECU.
//
//==========================================================================


#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <SPI.h>
#include <mcp_can.h>
#include <esp_task_wdt.h>

// CAMERA MODEL (AI THINKER)


#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


// MCP2515

#define MISO_PIN 12
#define MOSI_PIN 13
#define SCK_PIN 14
#define CS_PIN 15

MCP_CAN CAN(CS_PIN); // CS PIN

#define FLASH_LED 4
#define CONT_ID 0x100
#define ECU_PING_ID 0x110
#define VISION_ACK_ID 0x111
#define VISION_TEST_ID 0x107
#define VISION_TEST_ACK_ID 0x207
#define CAPTURE_ID 0x220


// ROI CONFIGURATION

// QVGA = 320x240

#define IMG_W 320
#define IMG_H 240

// Lower-middle region (vehicle headlights)
#define LOWER_Y_START 0
#define LOWER_Y_END   90

#define LOWER_X_START 20
#define LOWER_X_END   280


void setup()
{
    Serial.begin(115200);

    pinMode(FLASH_LED, OUTPUT);
    digitalWrite(FLASH_LED, LOW);
    
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0);
   
    // CAMERA CONFIG
    
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;

    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;

    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;

    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    config.xclk_freq_hz = 20000000;

    config.pixel_format = PIXFORMAT_GRAYSCALE;

    // LOW RESOLUTION = STABLE
    config.frame_size = FRAMESIZE_QVGA;

    config.jpeg_quality = 12;
    config.fb_count = 1;


    // INIT CAMERA
   

    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK)
    {
        Serial.println("Camera Init Failed");
        digitalWrite(FLASH_LED, HIGH);
        return;
    }

  
    // CAMERA SENSOR TUNING

    
    sensor_t * s = esp_camera_sensor_get();
    s->set_brightness(s,0);                //adjust if too dark/bright(-2 to 2)
    s->set_contrast(s,1);                   //increase to highlight headlights(-2 to 2)
    s->set_saturation(s,-2);                //not important for grayscale
    s->set_special_effect(s,0);            //grayscale mode
    
    // Disable automatic behaviors as much as possible

    s->set_whitebal(s, 0);
    s->set_awb_gain(s, 0);

    s->set_exposure_ctrl(s, 1);
    s->set_aec2(s, 0);
    s->set_ae_level(s, 0);
    
    s->set_gain_ctrl(s, 1);
    s->set_gainceiling(s, (gainceiling_t)1);
    
    //Image corrections
    s->set_bpc(s, 0);
    s->set_wpc(s, 0);
    s->set_raw_gma(s, 0);
    s->set_lenc(s, 0);
  
    //Image orientation
    s->set_hmirror(s, 0);
    s->set_vflip(s, 0);

    //Stability
    s->set_dcw(s, 1);


    //Test pattern OFF
    s->set_colorbar(s, 0);


    // CAN INIT
    
    SPI.begin(SCK_PIN,MISO_PIN,MOSI_PIN,CS_PIN);

    if(CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK) {
    Serial.println("CAN initialization failed...");
    //while(1);
  }
  else
  {
    digitalWrite(FLASH_LED, HIGH);
    delay(1000);
    digitalWrite(FLASH_LED, LOW);
  }
  CAN.setMode(MCP_NORMAL);
  Serial.println("ECU 1 (ESP32-CAM) READY");

  esp_task_wdt_init(5, true);
  esp_task_wdt_add(NULL);
}

// =========================================================
// MAIN LOOP
// =========================================================

void loop()
{
    checkCANCommand();
    esp_task_wdt_reset();
    camera_fb_t * fb = esp_camera_fb_get();

    if (!fb)
    {
        Serial.println("Frame Capture Failed");
        uint8_t captr[8];
        captr[0] = 0;
        CAN.sendMsgBuf(CAPTURE_ID, 0, 8, captr);
        return;
    }
    else
    {
       uint8_t captr[8];
       captr[0] = 1;
       CAN.sendMsgBuf(CAPTURE_ID, 0, 8, captr);
    }

    uint8_t *img = fb->buf;

    // =====================================================
    // LOWER-MIDDLE REGION ANALYSIS
    // =====================================================

    uint32_t roiSum = 0;
    uint32_t roiPixels = 0;
    uint32_t brightSum = 0;
    uint32_t brightCount = 0;
    const uint8_t BRIGHT_THRESHOLD = 200;

    for (int y = LOWER_Y_START; y < LOWER_Y_END; y += 2)
    {
        for (int x = LOWER_X_START; x < LOWER_X_END; x += 2)
        {
            int index = y * IMG_W + x;
            uint8_t val = img[index];
            roiSum += val;
            roiPixels++;
            
            if(val > BRIGHT_THRESHOLD)
            {
              brightSum += val;
              brightCount++;
            }
        }
    }
    uint8_t brightAvg = roiSum /roiPixels;

    if(brightCount > 255){
      brightCount = 255;
    }

    // CAN DATA
 
     uint8_t msg[8];
     msg[0] = brightAvg;
     msg[1] = brightCount;
     msg[2] = 0;
     msg[3] = 0;
     msg[4] = 0;
     msg[5] = 0;
     msg[6] = 0;
     msg[7] = 0;
     CAN.sendMsgBuf(CONT_ID, 0, 8, msg);
   
    // DEBUG

    Serial.print("BRIGHT AVG: ");
    Serial.print(brightAvg);

    Serial.print("  BRIGHT COUNT: ");
    Serial.println(brightCount);

    // Return frame buffer
    esp_camera_fb_return(fb);

    delay(100);
}

void checkCANCommand()
{
    if(CAN_MSGAVAIL == CAN.checkReceive())
    {
        long unsigned int rxId;
        byte len;
        byte rxBuf[8];

        CAN.readMsgBuf(&rxId, &len, rxBuf);


        if(rxId == ECU_PING_ID)
        {
            if(rxBuf[0]=='P' &&
               rxBuf[1]=='I' &&
               rxBuf[2]=='N' &&
               rxBuf[3]=='G')
            {
                sendVisionACK();
            }
        }

       if(rxId == VISION_TEST_ID)
        {
          digitalWrite(FLASH_LED, HIGH);
          delay(1000);
          digitalWrite(FLASH_LED, LOW);

          byte ack[1];
          ack[0] = 1;
          CAN.sendMsgBuf(VISION_TEST_ACK_ID, 0, 1, ack);
        } 
    }
}

void sendVisionACK()
{
  byte ackData[8];
  ackData[0] = 'A';
  ackData[1] = 'C';
  ackData[2] = 'K';

  CAN.sendMsgBuf(VISION_ACK_ID, 0, 3, ackData);
}
