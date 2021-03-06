#include <esp_now.h>
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
//#include "ESPAsyncWebServer.h"
//Provide your own WiFi credentials
const char* ssid = "Proximus-Home-84B0";//Proximus-Home-84B0   LARAS ousmane
const char* password = "w2eyafdmrh3re";//w2eyafdmrh3re   wifi4guest  kasskass
WiFiServer server(80);
//AsyncWebServer server1(80);
String header;

String eINK_ONE = "off";
String eINK_TWO = "off";
String eINK_THREE = "off";
String TEMPBXL="off";
String PRESBXL="off";
String TEMPTOR="off";
String PRETOR="off";

const int GPIO_PIN_NUMBER_22 = 22;
const int GPIO_PIN_NUMBER_23 = 23;
const int GPIO_PIN_NUMBER_15 = 15;


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
//france
String cityFR = "BRUXELLES";
String codeFR = "BXL";


// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;

String jsonBuffer;
//
float BxlTemperature=0.0;
float pressionBXL=0.0;
float TorontoTemperature=0.0;
float pressionTOR=0.0;

int temp_max;

int humid=0;
String country="";

bool sdOK = false;


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
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

void getTempPressionTORONTO(){
  //Initiate HTTP client
              HTTPClient http;
              //API
                String request = "http://api.openweathermap.org/data/2.5/weather?q=TORONTO,CA&APPID=" + openWeatherMapApiKey;
                //String request = "http://api.openweathermap.org/data/2.5/weather?q=Paris"  + "," +  "FR&APPID=" + openWeatherMapApiKey;
            
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

             //
             /*float BxlTemperature=0.0;
float pressionBXL=0.0;
float TorontoTemperature=0.0;*/
             //
              http.end();
              //display.print("Temp BXL:");
              TorontoTemperature=doc["main"]["temp"].as<float>()-273.15 ;
              //temp_max=doc["main"]["temp_max"].as<int>()-273.15 ; 
              pressionTOR=doc["main"]["pressure"].as<float>() ;
              //humid=doc["main"]["humidity"].as<int>() ;
              //country=doc["name"].as<String>() ;
              //String country="";
              Serial.println(BxlTemperature);
              Serial.println(pressionTOR);
              //Serial.println(Time);
              Serial.println(" ");
}
void getTempPressionBXL(String code)
{
  //Initiate HTTP client
  HTTPClient http;
  //API
    String request = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + code + "&APPID=" + openWeatherMapApiKey;
    //String request = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

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
  BxlTemperature=doc["main"]["temp"].as<float>()-273.15 ;
  //temp_max=doc["main"]["temp_max"].as<int>()-273.15 ; 
  pressionBXL=doc["main"]["pressure"].as<float>() ;
  //humid=doc["main"]["humidity"].as<int>() ;
  //country=doc["name"].as<String>() ;
  //String country="";
  Serial.println(BxlTemperature);
  Serial.println(pressionBXL);
  //Serial.println(Time);
  Serial.println(" ");
}

// Sending/Receiving example

HardwareSerial Sender(1);   // Define a Serial port instance called 'Sender' using serial port 1

//HardwareSerial Sender1(3); 

#define Sender_Txd_pin 21
#define Sender_Rxd_pin 22

void setup() {
  //Serial.begin(Baud Rate, Data Protocol, Txd pin, Rxd pin);
  //Serial.begin(9600);
  Serial.begin(115200);                                             // Define and start serial monitor
  Sender.begin(115200, SERIAL_8N1, Sender_Txd_pin, Sender_Rxd_pin); // Define and start Sender serial port
  //Sender1.begin(115200, SERIAL_8N1, Sender_Txd_pin, Sender_Rxd_pin);

  
  Serial.print("Connecting to ");
  Serial.println("LARAS");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Trying to connect to Wifi Network");
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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

  server.begin();
}

void loop() {
  //float sensor_temperature = 22.141;                               // Set an example value
  //Sender.print(sensor_temperature);                                // Send it to Sender serial port
  
  //delay(2000);

  //

 WiFiClient client = server.available(); 
      if (client) { 
        Serial.println("New Client is requesting web page"); 
        String current_data_line = ""; 
      while (client.connected()) { 
        if (client.available()) { 
          char new_byte = client.read(); 
          Serial.write(new_byte); 
          header += new_byte;
          if (new_byte == '\n') { 
                   
          if (current_data_line.length() == 0) 
          {
                      
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                      
          if (header.indexOf("TEMPBXL=ON") != -1) 
          {
            Serial.println("GPIO23 LED is ON");
            TEMPBXL = "on";
            String code="BXL";
            getTempPressionBXL(code);
            
            
            float sensor_temperature = BxlTemperature;                               // Set an example value
            Sender.print(sensor_temperature);                                // Send it to Sender serial port
            
            //delay(2000);
          } 

          if(header.indexOf("PRESBXL=ON") != -1){
             Serial.println("GPIO23 LED is ON");
            PRESBXL = "on";
            String code="BXL";
            getTempPressionBXL(code);
              //getTempFR();

              float sensor_pressionbxl = pressionBXL;                               // Set an example value
               Sender.print(sensor_pressionbxl);                                // Send it to Sender serial port
          }
          
          if (header.indexOf("LED0=OFF") != -1) 
          {
            Serial.println("GPIO23 LED is OFF");
            eINK_ONE = "off";
            //digitalWrite(GPIO_PIN_NUMBER_22, LOW);
          } 
          if (header.indexOf("TEMPTOR=ON") != -1)
          {
            Serial.println("GPIO23 LED is ON");
            TEMPTOR = "on";
          
             getTempPressionTORONTO();
            

               float sensor_temp = TorontoTemperature;                               // Set an example value
               Sender.print(sensor_temp);                                // Send it to Sender serial port
          }
          //
          if (header.indexOf("PRETOR=ON") != -1)
          {
            Serial.println("GPIO23 LED is ON");
            PRETOR = "on";
           
             getTempPressionTORONTO();
            

               float sensor_pression = pressionTOR;                               // Set an example value
               Sender.print(sensor_pression);                                 // Send it to Sender serial port
          }
          //
          if (header.indexOf("e-INK1=OFF") != -1) 
          {
            Serial.println("GPIO23 LED is OFF");
            eINK_TWO = "off";
            //digitalWrite(GPIO_PIN_NUMBER_23, LOW);
          }
          
                      
        client.println("<!DOCTYPE html><html>");
        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        client.println("<link rel=\"icon\" href=\"data:,\">");
        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
        client.println(".button { background-color: #4CAF50; border: 2px solid #4CAF50;; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; }");
        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"); 
        // Web Page Heading
        client.println("</style></head>");
        client.println("<body><center><h1>E-ink Web server  controller </h1></center>");
        /*client.println("<center><h2>Web Server Example Microcontrollerslab.com</h2></center>" );
        client.println("<center><h2>Press on button to turn on led and off button to turn off LED</h3></center>");*/
        client.println("<form><center>");
        client.println("<p> Temperature " + eINK_ONE + "</p>");
        // If the PIN_NUMBER_22State is off, it displays the ON button 
        client.println("<center> <button class=\"button\" name=\"TEMPBXL\" value=\"ON\" type=\"submit\">Temp BRUXELLES</button>") ;
        
        client.println("<button class=\"button\" name=\"PRESBXL\" value=\"ON\" type=\"submit\">PRESSSION BXL</button><br><br>");
        
        client.println("<p>TORONTO " + eINK_TWO + "</p>");
        client.println("<button class=\"button\" name=\"TEMPTOR\" value=\"ON\" type=\"submit\">TEMP Toronto ON</button>");
        client.println("<button class=\"button\" name=\"PRETOR\" value=\"ON\" type=\"submit\">Pression TORONTO ON</button>");
        /*client.println("<button class=\"button\" name=\"LED1\" value=\"OFF\" type=\"submit\">LED1 OFF</button> <br><br>");*/
        /*client.println("<p>third is " + eINK_THREE + "</p>");
        client.println ("<button class=\"button\" name=\"e-INK2\" value=\"ON\" type=\"submit\">eINK2 ON</button>");
        /*client.println ("<button class=\"button\" name=\"LED2\" value=\"OFF\" type=\"submit\">LED2 OFF</button></center>");*/
        client.println("</center></form></body></html>");
        client.println();
        break;
      } 
      else 
      { 
        current_data_line = "";
      }
      } 
      else if (new_byte != '\r') 
      { 
      current_data_line += new_byte; 
      }
      }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
      }
  //
}
