/**************************************************************************/
/*! 
    @file     iso14443a_uid.pde
    @author   Adafruit Industries
	@license  BSD (see license.txt)

    This example will attempt to connect to an ISO14443A
    card or tag and retrieve some basic information about it
    that can be used to determine what type of card it is.   
   
    Note that you need the baud rate to be 115200 because we need to print
	out the data and read from the card at the same time!

This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
This library works with the Adafruit NFC breakout 
  ----> https://www.adafruit.com/products/364
 
Check out the links above for our tutorials and wiring diagrams 
These chips use SPI or I2C to communicate.

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include <LiquidCrystal.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a SPI connection:
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

const int rs = 22, en = 23, d4 = 24, d5 = 25, d6 = 26, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
//Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("HCE Shaxzod");
  
  Serial.begin(115200);
  delay(1000);
    
    lcd.clear();
    lcd.println("Peer to Peer HCE");
    
  
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
    
    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    //nfc.setPassiveActivationRetries(0xFF);
    
    // configure board to read RFID tags
    nfc.SAMConfig();
}

void loop(void) {
  lcd.clear();
  bool success;
  
  uint8_t responseLength = 32;
  
  lcd.println("Waiting for an ISO14443A card");
  
  // set shield to inListPassiveTarget
  success = nfc.inListPassiveTarget();

  if(success) {
    lcd.clear();
     lcd.println("Found something!");
                  
    uint8_t selectApdu[] = { 0x00, /* CLA */
                              0xA4, /* INS */
                              0x04, /* P1  */
                              0x00, /* P2  */
                              0x07, /* Length of AID  */
                              0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* AID defined on Android App */
                              0x00  /* Le  */ };
                              
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
}

void printResponse(uint8_t *response, uint8_t responseLength) {
  
   String respBuffer;

    for (int i = 0; i < responseLength; i++) {
      
      if (response[i] < 0x10) 
        respBuffer = respBuffer + "0"; //Adds leading zeros if hex value is smaller than 0x10
      
      respBuffer = respBuffer + String(response[i], HEX) + " ";                        
    }
    lcd.clear();
    lcd.print("response: "); lcd.println(respBuffer);
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
