/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/
#include <WebServer.h>
#include <esp_now.h>
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
//new API
#include <Arduino_JSON.h>
//
#include <Arduino.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <stdio.h>
#define MAX 100
float tempC;
float changeMoney;

AsyncWebServer server(80);
const char* host = "free.currconv.com";
const int httpsPort = 443; //http port

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "Proximus-Home-84B0";//Proximus-Home-84B0   LARAS ousmane
const char* password = "w2eyafdmrh3re";//w2eyafdmrh3re   wifi4guest  kasskass

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";
const char* PARAM_INPUT_4 = "input4";


//
WiFiClientSecure client;
const char* test_root_ca = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
                           "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
                           "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
                           "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
                           "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
                           "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
                           "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
                           "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
                           "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
                           "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
                           "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
                           "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
                           "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
                           "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
                           "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
                           "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
                           "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
                           "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
                           "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n" \
                           "-----END CERTIFICATE-----\n";

//
String response = "";
//JSON document
DynamicJsonDocument doc(2048);
//Wheather API
// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "b8890bd0a9fe6b0bccc79b716cbdcd2f";

//
char buf[MAX];
//

//
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
/*7C:9E:BD:FA:E9:FC 
7C:9E:BD:FA:F3:E0
7C:9E:BD:FA:EB:F4*/
uint8_t broadcastAddress1[] = {0x7C, 0x9E, 0xBD, 0xFA, 0xE9, 0xFC};
uint8_t broadcastAddress2[] = {0x7C, 0x9E, 0xBD, 0xFA, 0xF3, 0xE0 };
uint8_t broadcastAddress3[] = {0x7C, 0x9E, 0xBD, 0xFA, 0xEB, 0xF4 };

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  float dataToSend;
} struct_message;

// Create a struct_message called myData
struct_message myData;
struct_message myData2;
struct_message myData3;
struct_message myData4;
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h1>E-ink1</h1>
  <form action="/get">
    Entrer city(en celcuis) : <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    currency conversion( ex: USD_EUR ): <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>
  <h1>E-ink2</h1>
  <form action="/get">
    Entrer city : <input type="text" name="input3">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    currency conversion( ex: USD_EUR ): <input type="text" name="input4">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  //
if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // register peer
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
    
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  //
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    //
    float tempMeteo;

    float change;
    //
     //Initiate HTTP client
     HTTPClient http;
    //
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      String request = "http://api.openweathermap.org/data/2.5/weather?q="+inputMessage+"&APPID=" + openWeatherMapApiKey;
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
      
      tempMeteo=doc["main"]["temp"].as<float>()-273.15;
      //char buf[MAX];
      gcvt(tempMeteo, 6, buf);
      inputParam = PARAM_INPUT_1;

      //
      myData.dataToSend=tempMeteo;
      
             esp_err_t result1 = esp_now_send(
                  broadcastAddress2, 
                  (uint8_t *) &myData,
                  sizeof(myData));
                 
                if (result1 == ESP_OK) {
                  Serial.println("Sent with success");
                }
                else {
                  Serial.println("Error sending the data");
                }

        //delay(500);
      //
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      
      Serial.println("");
      client.setCACert(test_root_ca);
     
      Serial.println("Ready");
      client.stop();
      if (client.connect(host, httpsPort)) {
        String url = "/api/v7/convert?q="+inputMessage+"&compact=ultra&apiKey=07a4e8fa6892a811725e";
        client.print(String("GET ") + url + " HTTP/1.0\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP32\r\n" + "Connection: close\r\n\r\n");
        while (client.connected()) {
          String line = client.readStringUntil('\n');
          if (line == "\r") {
            break;
          }
        }
        String line = client.readStringUntil('\n');
        String abc = line.substring(11, 20);
        Serial.println("Kurz (orezany text): ");
        Serial.println(abc);
        float kurz = abc.toFloat();
        Serial.println("Kurz (float): ");
        change =kurz;
        myData.dataToSend=change;
      
             esp_err_t result1 = esp_now_send(
                  broadcastAddress2, 
                  (uint8_t *) &myData,
                  sizeof(myData));
                 
                if (result1 == ESP_OK) {
                  Serial.println("Sent with success");
                }
                else {
                  Serial.println("Error sending the data");
                }
       
      
      }
    inputParam = PARAM_INPUT_2;
  }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();

      String request = "http://api.openweathermap.org/data/2.5/weather?q="+inputMessage+"&APPID=" + openWeatherMapApiKey;
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
      
      tempMeteo=doc["main"]["temp"].as<float>()-273.15;
      //char buf[MAX];
      gcvt(tempMeteo, 6, buf);
      inputParam = PARAM_INPUT_1;

      //
      myData.dataToSend=tempMeteo;
      
             esp_err_t result1 = esp_now_send(
                  broadcastAddress3, 
                  (uint8_t *) &myData,
                  sizeof(myData));
                 
                if (result1 == ESP_OK) {
                  Serial.println("Sent with success");
                }
                else {
                  Serial.println("Error sending the data");
                }
      inputParam = PARAM_INPUT_3;
    }
    else if(request->hasParam(PARAM_INPUT_4)){
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
       Serial.println("");
      client.setCACert(test_root_ca);
     
      Serial.println("Ready");
      client.stop();
      if (client.connect(host, httpsPort)) {
        String url = "/api/v7/convert?q="+inputMessage+"&compact=ultra&apiKey=07a4e8fa6892a811725e";
        client.print(String("GET ") + url + " HTTP/1.0\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP32\r\n" + "Connection: close\r\n\r\n");
        while (client.connected()) {
          String line = client.readStringUntil('\n');
          if (line == "\r") {
            break;
          }
        }
        String line = client.readStringUntil('\n');
        String abc = line.substring(11, 20);
        Serial.println("Kurz (orezany text): ");
        Serial.println(abc);
        float kurz = abc.toFloat();
        Serial.println("Kurz (float): ");
        change =kurz;
        myData.dataToSend=change;
      
             esp_err_t result1 = esp_now_send(
                  broadcastAddress3, 
                  (uint8_t *) &myData,
                  sizeof(myData));
                 
                if (result1 == ESP_OK) {
                  Serial.println("Sent with success");
                }
                else {
                  Serial.println("Error sending the data");
                }
      
      }
      inputParam = PARAM_INPUT_4;
    }else{
      inputMessage = "No message sent";
      inputParam = "none";
    }
    //doc
    //Serial.println(doc);
    //Serial.println(tempMeteo);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + myData.dataToSend +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  
}
