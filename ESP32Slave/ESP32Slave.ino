/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/



#include <esp_now.h>
#include <WiFi.h>
//
#include <GxEPD.h>
#include "SD.h"
#include "SPI.h"


#include <WiFi.h>
#include "time.h"

const char* ssid = "Proximus-Home-84B0";//Proximus-Home-84B0   LARAS ousmane
const char* password = "w2eyafdmrh3re";//w2eyafdmrh3re   wifi4guest  kasskass

/*const char* ntpServer = "ntp.ntsc.ac.cn";//local ntp server
const uint32_t  gmtOffset_sec = 8*3600;  //GMT+08:00
const uint16_t   daylightOffset_sec = 0;*/
//Since there are multiple versions of the screen, if there is a flower screen after downloading the program, please test the following four header files again!
//#include <GxDEPG0213BN/GxDEPG0213BN.h>
//#include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w
//#include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w new panel
#include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w newer panel


#include <Fonts/FreeMonoBold18pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#define SPI_MOSI 23
#define SPI_MISO -1
#define SPI_CLK 18

#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17

#define SDCARD_SS 13
#define SDCARD_CLK 14
#define SDCARD_MOSI 15
#define SDCARD_MISO 2

#define BUTTON_PIN 39

typedef enum
{
    RIGHT_ALIGNMENT = 0,
    LEFT_ALIGNMENT,
    CENTER_ALIGNMENT,
} Text_alignment;



GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);

SPIClass sdSPI(VSPI);

const uint8_t Whiteboard[1700] = {0x00};

/*uint16_t Year = 0 , Month = 0 , Day = 0 , Hour = 0 , Minute = 0 , Second = 0;
char Date[]={"2000/01/01"};
char Time[]={"00:00:00"};*/
bool sdOK = false;

void displayText(const String &str, uint16_t y, uint8_t alignment)
{
  int16_t x = 0;
  int16_t x1, y1;
  uint16_t w, h;
  display.setCursor(x, y);
  display.getTextBounds(str, x, y, &x1, &y1, &w, &h);

  switch (alignment)
  {
  case RIGHT_ALIGNMENT:
    display.setCursor(display.width() - w - x1, y);
    break;
  case LEFT_ALIGNMENT:
    display.setCursor(0, y);
    break;
  case CENTER_ALIGNMENT:
    display.setCursor(display.width() / 2 - ((w + x1) / 2), y);
    break;
  default:
    break;
  }
  display.println(str);
}


//}

//
//Structure example to receive data
//Must match the sender structure
typedef struct test_struct {
  float x;
  /*int y;*/
} test_struct;

//Create a struct_message called myData
test_struct myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

 //

  displayText(String(myData.x), 60, CENTER_ALIGNMENT);
  //displayText(String(myData.y), 90, CENTER_ALIGNMENT);
  display.updateWindow(22, 30,  222,  90, true);
  display.drawBitmap(Whiteboard, 22, 31,  208, 60, GxEPD_BLACK);
 //
  
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(myData.x);
  /*Serial.print("y: ");
  Serial.println(myData.y);
  Serial.println();*/
}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);


  //
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
  display.init(); // enable diagnostic output on Serial

  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(0, 0);

  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

  if (!SD.begin(SDCARD_SS, sdSPI)) {
    sdOK = false;
  } else {
    sdOK = true;
  }

  display.fillScreen(GxEPD_WHITE);
  display.update();

  //
}
 
void loop() {

}
