#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

#include "default_qr.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     A3 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16


const byte ROWS = 4, COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


#define WAIT 0
#define GET_QR 1
#define SHOW_QR 2
#define OPEN_DOOR 3


unsigned char state;

void print_char(char c);
void print_str(char* c);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(100);

  state = WAIT;

}

void loop() {
  switch (state)
  {
  case WAIT:
  {
     print_str("enter 'A' to get QR code");

    char customKey = customKeypad.getKey();

    if(customKey)
    {

      print_char(customKey);
      if(customKey == 'A')
      {
            state = GET_QR;
      }
    }
  
    break;
  }
  case GET_QR:
  {
    //TODO: ask server for a qr code and wait for server response 
    //then go to show_qr state
    print_str("please wait...");
    delay(1500);
    state= SHOW_QR;
    break;
  }
  case SHOW_QR:
  {
     display.clearDisplay(); //for Clearing the display
     display.drawBitmap(0, 0, default_QR, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
     display.display();
     delay(2000);

    //TODO: show the qr
    //then wait for server to send response
    //wait for xx seconds 
    //if response was go to state open door
    //else go to idle (and print message on lcd)
    state = OPEN_DOOR;
    break;
  }
  case OPEN_DOOR:
  {
      print_str("opening...");
      delay(1500);

      //TODO: open the door, wait for xx seconds, close the door
      state = WAIT;
    break;
  }
  default:
  {
  print_str("???");
  delay(500);
    break;
  }
  }
}

void print_char(char c)
{
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write(c);
  display.display();
 }

 void print_str(char* c)
 {

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println(c);
  display.display();
  }
