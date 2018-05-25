// Shakh
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "SerialManager.h"

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <math.h>
#include <string.h>

// ----------- NFC Module config -------------- //
// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
bool nfc_success;
String ID = "13";
String NAME = "MCHJ Shaxzod";
String repeat_request_string = "repeat";
uint8_t REPEAT_REQUEST[6];
String AUTH_KEY = "qwerty";
String EXP_DATE = "12/19";




// Use this line for a breakout with a SPI connection:
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// ----------- LCD Distplay config -------------- //
const int rs = 22, en = 23, d4 = 24, d5 = 25, d6 = 26, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// ----------- Serial config -------------- //
SerialManager serial;

// ----------- Keypad config -------------- //
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
char keypad_keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'7', '8', '9', '-'}, // '-' -> Backspace
  {'4', '5', '6', 'c'}, // 'c' -> Clear
  {'1', '2', '3', 'p'}, // 'p' -> Power ON/OFF
  {'d', '0', '.', 'o'}  // 'd' -> Double zero, 'o' -> OK
};

byte keypad_row_pins[KEYPAD_ROWS] = {40, 42, 44, 46};
byte keypad_col_pins[KEYPAD_COLS] = {41, 43, 45, 47};

Keypad keypad = Keypad(makeKeymap(keypad_keys), keypad_row_pins, keypad_col_pins, KEYPAD_ROWS, KEYPAD_COLS); // init keypad


bool is_ready = false;
bool is_enter_number = false;

long number = 0;

void setup(void) {
  repeat_request_string.getBytes(REPEAT_REQUEST, repeat_request_string.length()+1);
  
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("HCE Shaxzod");
  
  delay(1000);

  setupNFC(); // Setting up NFC Module

  serial.start(1); // Working with Serial1
  Serial.begin(115200);
}

void loop(void) {

  if (serial.onReceive()){
    lcd.clear();
    switch (serial.getCmd().toInt()){
      case 0:
        switch (serial.getParam().toInt()){
          case 0:
            lcd.println("Connecting to wifi");
            is_ready = false;
          break;
          case 1:
            lcd.println("WiFi connected");
            is_ready = false;
          break;
          case 2:
            lcd.println("Connection timeout");
            is_ready = false;
          break;
        }
        break;
        case 1:
          switch (serial.getParam().toInt()){
            case 0:
              lcd.println("Connecting to server");
              is_ready = false;
            break;  
            case 1:
              lcd.print("Ready");
              is_ready = true;
              is_enter_number = true;
            break;
            case 2:
              lcd.println("Expiry date");
              is_ready = false;
            break;
            case 3:
              lcd.println("Connection failed");
              is_ready = false;
            break;
          }
        break;
        case 2:
          lcd.println(serial.getParam());
        break;
        case 3:
          switch (serial.getParam().toInt()){
            case 0:
              lcd.println("Request...");
              is_ready = false;
            break;
            case 1:
              lcd.println("OK");
              is_ready = true;
            break;
            case 2:
              lcd.println("Request timeout");
              is_ready = false;
            break;
            case 3:
              lcd.println("Request cancelled");
              is_ready = false;
            break;
            default:
              lcd.println(serial.getParam());
              delay(1000);
            break;
          }
        break;
    }
  }

  if (is_ready){
    lcd.setCursor(0,0);
    lcd.print("Enter amount: ");
     // Start entering number from Keypad
    lcd.setCursor(0, 1);
    lcd.print(number);



      if (is_enter_number){
        switch (keypad.getKey()){
          case '0':
            if (number != 0){
              number *= 10;
            }
          break;
          case '1':
            if (number == 0){
              number = 1;
            }
            else {
              number = (number * 10) + 1;
            }
          break;
          case '2':
            if (number == 0){
              number = 2;
            }
            else {
              number = (number * 10) + 2;
            }
          break;
          case '3':
            if (number == 0){
              number = 3;
            }
            else {
              number = (number * 10) + 3;
            }
          break;
          case '4':
            if (number == 0){
              number = 4;
            }
            else {
              number = (number * 10) + 4;
            }
          break;
          case '5':
            if (number == 0){
              number = 5;
            }
            else {
              number = (number * 10) + 5;
            }
          break;
          case '6':
            if (number == 0){
              number = 6;
            }
            else {
              number = (number * 10) + 6;
            }
          break;
          case '7':
            if (number == 0){
              number = 7;
            }
            else {
              number = (number * 10) + 7;
            }
          break;
          case '8':
            if (number == 0){
              number = 8;
            }
            else {
              number = (number * 10) + 8;
            }
          break;
          case '9':
            if (number == 0){
              number = 9;
            }
            else {
              number = (number * 10) + 9;
            }
          break;
          case 'c':
            number = 0;
          break;
          case '-':
            number = trunc(number/10);
          break;
          case 'o':
            if (number > 0){
              is_enter_number = false;
              // activate nfc reader data exchange -----------------------------------------

              do {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(number);
              lcd.print(" UZS");
              lcd.setCursor(0, 1);
              lcd.print("Waiting for phone...");
              bool nfc_passivetarget;
              do {
                nfc_passivetarget = nfc.inListPassiveTarget();
                if (nfc_passivetarget){
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print(number); lcd.print(" UZS");
                  lcd.setCursor(0, 1);
                  lcd.print("Phone placed to NFC");

                  uint8_t select_apdu_aid[] = { 0x00, // CLA 
                              0xA4, // INS
                              0x04, // P1 
                              0x00, // P2 
                              0x07, // Length of AID  
                              0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // AID defined on Android App 
                              0x00  // Le  
                              };

                  uint8_t response[60];
                  uint8_t response_length = 60;
                  
                  nfc_success = nfc.inDataExchange(select_apdu_aid, sizeof(select_apdu_aid), response, &response_length);
                  if (nfc_success){
                     
                     String imterminal_string = "{\"c\":0,\"t\":" + ID + ", \"n\":\""+NAME+"\"}";
                     uint8_t imterminal[imterminal_string.length()+1];
                     imterminal_string.getBytes(imterminal,imterminal_string.length()+1);
                     uint8_t imt_response_length = 60;
                     uint8_t imt_response[imt_response_length];
                     
                     nfc_success = nfc.inDataExchange(imterminal, sizeof(imterminal), imt_response, &imt_response_length);
                     if (nfc_success){
                        String auth_key = (char*)imt_response;
                        auth_key.remove(auth_key.length()-1);

                        String ineed_string = "{\"c\":1,\"s\":"+String(number)+"}";
                        uint8_t ineed_response_length = 60;
                        uint8_t ineed_response[ineed_response_length];
                        uint8_t ineed[ineed_string.length()+1];
                        ineed_string.getBytes(ineed, ineed_string.length()+1);

                        do {
                          nfc_success = nfc.inDataExchange(ineed, sizeof(ineed), ineed_response, &ineed_response_length);
                          if (nfc_success && ineed_response != REPEAT_REQUEST){
                            String answer = (char*)ineed_response;
                            answer.remove(answer.length()-1);
                            if (answer == "yes"){
                              lcd.print("yes");
                              String transaction_request = "0,cards/terminaltransaction&auth_key=" + AUTH_KEY + "&user_auth_key="+auth_key;
                              serial.println(transaction_request);
                              // Alert ----------------                             
                            }
                            else {
                              lcd.print("no");
                            }
                            is_enter_number = true;
                            number = 0;
                          }
                        }
                        while (ineed_response == REPEAT_REQUEST);
                     }
                  }
                  else {
                    // Alert ....
                  }
                }
              } while (!nfc_passivetarget);
            
              }while (true);
              /*
              is_enter_number = true;
              number = 0;
              lcd.clear();*/
            }
          break;
        }
      }
  }

  

  

  delay(100);
  
  /*
  lcd.clear();
  
  bool success;
  
  uint8_t responseLength = 32;
  
  lcd.println("Waiting for an ISO14443A card");
  
  // set shield to inListPassiveTarget
  success = nfc.inListPassiveTarget();

  if(success) {
    lcd.clear();
     lcd.println("Found something!");
                  
    uint8_t selectApdu[] = { 0x00, // CLA 
                              0xA4, // INS
                              0x04, // P1 
                              0x00, // P2 
                              0x07, // Length of AID  
                              0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // AID defined on Android App 
                              0x00  // Le  
                              };
                              
    uint8_t response[32];  
     
    success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);
    
    if(success) {
      lcd.clear();
      lcd.print("respLength: "); lcd.println(responseLength);
       
      nfc.PrintHexChar(response, responseLength);
      
      do {
        uint8_t apdu[] = "Hello from Arduino";
        uint8_t back[32];
        uint8_t length = 32; 

        success = nfc.inDataExchange(apdu, sizeof(apdu), back, &length);
        
        if(success) {
         lcd.clear();
          lcd.print("respLength: "); lcd.println(length);
           
          nfc.PrintHexChar(back, length);
        }
        else {
          lcd.clear();
          lcd.println("Broken connection?"); 
        }
      }
      while(success);
    }
    else {
     lcd.clear();
      lcd.println("Failed sending SELECT AID"); 
    }
  }
  else {
   lcd.clear();
    lcd.println("Didn't find anything!");
  }

  delay(1000);

  */

 /* 
  // String to byte array conversation-------------------
  String str = "Shakh";
  byte bytes[256];
  str.getBytes(bytes, sizeof(bytes));

  // byte array to String conversation-------------------
  byte b[] {
    0x01, 0x02, 0x00, 0x03
  };
  String s;
  for (int i = 0; i<sizeof(b); i++){
    s = s+String(b[i]);
  }*/
}


void printResponse(uint8_t *response, uint8_t responseLength) {
  
   String respBuffer;

    for (int i = 0; i < responseLength; i++) {
      
      if (response[i] < 0x10) 
        respBuffer = respBuffer + "0"; //Adds leading zeros if hex value is smaller than 0x10
      
      respBuffer = respBuffer + String(response[i], HEX) + " ";                        
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println(respBuffer);
}

void setupNFC() {
 
  nfc.begin();
    
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    lcd.clear();
    lcd.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  lcd.clear();
  lcd.print("Found chip PN5"); lcd.println((versiondata>>24) & 0xFF, HEX); 
  lcd.print("Firmware ver. "); lcd.print((versiondata>>16) & 0xFF, DEC); 
  lcd.print('.'); lcd.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig(); 
}

