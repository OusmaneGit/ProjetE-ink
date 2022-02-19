// include library, include base class, make path known
#include <GxEPD.h>
#include "SD.h"
#include "SPI.h"
//WIFI include
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//Since there are multiple versions of the screen, if there is a flower screen after downloading the program, please test the following four header files again!
//#include <GxDEPG0213BN/GxDEPG0213BN.h>
//#include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w
//#include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w new panel
#include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w newer panel
int bmpWidth = 232, bmpHeight = 52;
//width:150,height:39

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>


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


GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);

SPIClass sdSPI(VSPI);

//Provide your own WiFi credentials
const char* ssid = "LARAS";//Proximus-Home-84B0   LARAS
const char* password = "wifi4guest";//w2eyafdmrh3re   wifi4guest
//String for storing server response
String response = "";
//JSON document
DynamicJsonDocument doc(2048);

const char *Website = "Test API";
bool sdOK = false;
int startX = 9, startY = 20;

void setup()
{
  //For displaying the joke on Serial Monitor
  Serial.begin(9600);
  //Initiate WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected with IP: ");
  display.println(WiFi.localIP());
  //

//Initiate HTTP client
  HTTPClient http;
  //The API URL
  String request = "https://api.chucknorris.io/jokes/random";
  //Start the request
  http.begin(request);
  //Use HTTP GET request
  http.GET();
  //Response from server
  response = http.getString();
  //Parse JSON, read error if any
  DeserializationError error = deserializeJson(doc, response);
  if(error) {
     Serial.print(F("deserializeJson() failed: "));
     Serial.println(error.f_str());
     return;
  }
  //Print parsed value on Serial Monitor
  Serial.println(doc["value"].as<char*>());
  //Close connection  
  http.end();
  //Wait two seconds for next joke
  
   //
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
  display.init(); // enable diagnostic output on Serial

  display.setRotation(1);
  //
//  display.fillScreen(GxEPD_BLACK);
//  display.setTextColor(GxEPD_WHITE);
  //
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  //display.setFont(&FreeMonoBold12pt7b);
  //display.setCursor(0, 0);

  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

  if (!SD.begin(SDCARD_SS, sdSPI)){
  sdOK = false;
  } else {
  sdOK = true;
  }

  

  

  //display.drawBitmap(DFRobot, startX, startY,  bmpWidth, bmpHeight, GxEPD_BLACK);

  //display.setCursor(16,60);

  delay(2000);
  display.fillScreen(GxEPD_WHITE);
  display.println(doc["value"].as<char*>());

  //display.setTextColor(GxEPD_BLACK);

  //display.fillScreen(GxEPD_WHITE);
  //display.update();
  display.update();

  
  Serial.flush();
  // goto sleep
  //esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);

  //esp_deep_sleep_start();
  
}

void loop()
{
  delay(8000);
}
