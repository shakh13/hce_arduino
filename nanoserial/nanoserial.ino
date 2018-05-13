#include "ESP8266WiFi.h"
#include <string.h>
#include "SerialManager.h"
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

SerialManager serial;


const char *ssid = "ssh";
const char *password = "13ssh6006960";
String auth_key = "qwerty";
String exp_date = "12/19";
IPAddress wifi_ip(192, 168, 1, 153);
IPAddress wifi_subnet(255, 255, 255, 2);
IPAddress wifi_gateway(192, 168, 1, 1);
IPAddress wifi_dns(8, 8, 8, 8);


const char *host = "192.168.1.130";
String baseurl = "/hce/backend/web/?r=api/";
const int port = 80;
HTTPClient http;



String urls[1024] = {"cards/getcash", "cards/getactive"};
char url_id[10];



void setup() {

  int wifi_timeout;
  DynamicJsonBuffer jsonBuffer;
  

  // put your setup code here, to run once:
  serial.start(0);
  //serial.setFlag('>');
  //serial.setDelimiter('<');

  WiFi.mode(WIFI_STA);
  WiFi.config(wifi_ip, wifi_dns, wifi_gateway, wifi_subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    wifi_timeout++;
    if (wifi_timeout == 30) {
      serial.println("0,2"); // Timeout connecting to wifi
      while (true);
    }
    serial.println("0,0"); // Connecting to wifi
  }

  serial.println("0,1"); // Connected to wifi

  serial.println("1.0"); // Connectiong to the server
  http.begin("http://192.168.1.130/hce/backend/web/?r=api/terminal/login&auth_key=" + auth_key + "&exp_date=" + exp_date);
  int httpcode = http.GET();

  if (httpcode > 0){
    String payload = http.getString();
    //check exp_date
    JsonObject& json = jsonBuffer.parseObject(payload);

    if (json[String("status")].as<bool>() == true){
      serial.println("1,1"); 
    }
    else {
      serial.println("1.2");
      return;
    }
  }
  else {
    serial.println("1,3");
  }

  http.end();

}


void loop() {
  if (serial.onReceive()) {
    serial.println("---Start---");
    serial.println(serial.getCmd()/*.toInt()*/);
    serial.println(serial.getParam());
    //serial.println(serial.getValue());
    serial.println("---End---");
  }
}

