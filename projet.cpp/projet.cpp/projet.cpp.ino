#include <GxEPD.h>
#include "SD.h"
#include "SPI.h"


#include <WiFi.h>
#include <WebServer.h>
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
//new API
#include <Arduino_JSON.h>

//Provide your own WiFi credentials
const char* ssid = "Proximus-Home-84B0";//Proximus-Home-84B0   LARAS
const char* password = "w2eyafdmrh3re";//w2eyafdmrh3re   wifi4guest

/* Put IP Address details */
//IPAddress local_ip(172,30,40,28);
//IPAddress gateway(172,30,40,1);
//IPAddress subnet(255,255,255,0);

//WebServer server(80);


//Since there are multiple versions of the screen, if there is a flower screen after downloading the program, please test the following four header files again!
//#include <GxDEPG0213BN/GxDEPG0213BN.h>
//#include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w
//#include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w new panel
#include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w newer panel
//#include <GDEW0371W7/GDEW0371W7.h>
//#include <GxGDEW0213Z16/GxGDEW0213Z16.h>
//conversion
#include <iostream>
#include <string>
//
// FreeFonts from Adafruit_GFX
/*#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>*/
//
//#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

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
//
String response = "";
//JSON document
DynamicJsonDocument doc(2048);

const char *Website = "Test API";
//bool sdOK = false;
int startX = 9, startY = 20;
//
//Wheather API
// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "b8890bd0a9fe6b0bccc79b716cbdcd2f";
// Example:
//String openWeatherMapApiKey = "bd939aa3d23ff33d3c8f5dd1dd435";

// Replace with your country code and city
String city = "BRUXELLES";
String countryCode = "BXL";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;

String jsonBuffer;
//
int Temp=0;
int temp_max;
int pression=0;
int humid=0;
String country="";

bool sdOK = false;

void displayText(const String &str, uint16_t y, uint8_t alignment)
{
  int16_t x = 0;
  int16_t x1, y1;
  uint16_t w, h;
  //display.setCursor(20,95);
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
  //display.setCursor(40,95);
  display.println(str);
}

void getTemp()
{
  //Initiate HTTP client
  HTTPClient http;
  //API
    String request = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
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
 
  http.end();
  //display.print("Temp BXL:");
  Temp=doc["main"]["temp"].as<int>()-273.15 ;
  temp_max=doc["main"]["temp_max"].as<int>()-273.15 ; 
  pression=doc["main"]["pressure"].as<int>() ;
  humid=doc["main"]["humidity"].as<int>() ;
  country=doc["name"].as<String>() ;
  //String country="";
  Serial.println(Temp);
  //Serial.println(Time);
  Serial.println(" ");
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

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
  display.setFont(&FreeMonoBold12pt7b);
  //display.setCursor(, 0);
  display.setCursor(60,95);
  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

  if (!SD.begin(SDCARD_SS, sdSPI)) {
    sdOK = false;
  } else {
    sdOK = true;
  }

  display.fillScreen(GxEPD_WHITE);
  display.update();
}

void loop()
{
  delay(2000);
  getTemp();
  displayText("      "+String(Temp)+ "°C" , 120, LEFT_ALIGNMENT);
  //displayText(String(pression), 117, RIGHT_ALIGNMENT);
  displayText(String(country), 80, CENTER_ALIGNMENT);
  displayText("   max   "+String(temp_max)+ "°C" , 40, LEFT_ALIGNMENT);
  //displayText(String(humid), 35, RIGHT_ALIGNMENT);
   // Send an HTTP GET request
  display.updateWindow(22, 30,  222,  90, true);
  //display.drawBitmap(Whiteboard, 22, 31,  208, 60, GxEPD_BLACK);
  
}
