#include "ESP8266WiFi.h"
#include <string.h>
#include "SerialManager.h"
#include "RestClient.h"
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

SerialManager serial;


const char *ssid = "SHAKH";
const char *password = "shakh0013";
String auth_key = "qwerty";
String exp_date = "12/19";
//IPAddress wifi_ip(192, 168, 1, 129);
//IPAddress wifi_subnet(255, 255, 255, 0);
//IPAddress wifi_gateway(192, 168, 1, 1);
//IPAddress wifi_dns(8, 8, 8, 8);

String baseurl = "http://192.168.43.229/hce/backend/web/?r=api/";

HTTPClient http;
int httpcode;
String request;

bool checkServerTerminal(){
  serial.println("1.0"); // Connectiong to the server
  http.begin(baseurl+"terminal/login&auth_key=" + auth_key + "&exp_date=" + exp_date);
  httpcode = http.GET();
  

  if (httpcode > 0){
    String payload = http.getString();
    //check exp_date
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(payload);

    if (json[String("status")].as<bool>() == true){
      serial.println("1,1"); 
    }
    else {
      serial.println("1.2");
      return false;
    }
  }
  else {
    serial.println("1,3");
  }

  http.end();
}

void setup() {

  
  serial.start(0);

  delay(5000);
  int wifi_timeout;

  // put your setup code here, to run once:
  
  serial.setFlag('>');
  //serial.setDelimiter('<');

  WiFi.mode(WIFI_STA);
  //WiFi.config(wifi_ip, wifi_dns, wifi_gateway, wifi_subnet);
  WiFi.begin(ssid, password);

  delay(5000);
  serial.start(0);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    wifi_timeout++;
    if (wifi_timeout == 30) {
      serial.println("0,2"); // Timeout connecting to wifi
      while (true);
    }
    serial.println("0,0"); // Connecting to wifi
  }

  serial.println("0,1"); // Connected to wifi

  if (!checkServerTerminal()) return; // Server is not available or error with terminal or etc errors...

}

void loop() {

  HTTPClient r;
  if (serial.onReceive()) {
    switch (serial.getCmd().toInt()){
      case 0: // Request

        request = baseurl+"cards/terminaltransaction&auth_key=qwerty&user_auth_key="+serial.getParam();
        
        r.begin(request);  
        httpcode = r.GET();
        if (httpcode > 0){
          String response = "3,"+r.getString();
          serial.println(response);
        }
        else 
          serial.println("3,2");

        r.end();
        
      break;
      case 1: // Check WiFi
        serial.println(WiFi.status() == WL_CONNECTED ? "0,1" : "0,2");
      break;
      case 2:
        if (!checkServerTerminal()){
          return;
        }
      break;        
    }
  }
  delay(100);
}


