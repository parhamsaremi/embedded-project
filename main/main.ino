

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <qrcode.h>

/* LCD */

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     A3 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


/* KEYPAD */
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


/* RELAY */

const int relay_1 = 13;
const int relay_2 = 12;


/* STATES */

#define WAIT 0
#define GET_QR 1
#define SHOW_QR 2
#define OPEN_DOOR 3

unsigned char state;


/* FUNCTIONS */

void print_char(char c);
void print_str(char* c);
void PrintQRCode(const char * url);


/* SETUP */

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
  
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);

  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);

}


/* LOOP */

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
    delay(100);  

    state= SHOW_QR;
    break;
  }
  case SHOW_QR:
  {
     PrintQRCode("Embedded project :D");
     delay(2000);

    //TODO: show the qr
    //then wait for server to send response
    //wait for xx seconds 
    //if response was go to state open door
    //else go to wait (and print message on lcd)
    
    state = OPEN_DOOR;
    break;
  }
  case OPEN_DOOR:
  {
      print_str("opening...");
      
      digitalWrite(relay_1, LOW);
      digitalWrite(relay_2, HIGH);
      delay(1000);
      
      print_str("closing...");
      digitalWrite(relay_1, HIGH);
      digitalWrite(relay_2, LOW);
      
      delay(1000);
      digitalWrite(relay_1, HIGH);
      digitalWrite(relay_2, HIGH);
  
      //TODO: open the door, wait for xx seconds, close the door
      state = WAIT;
    break;
  }
  
  }
}

void print_char(char c)
{
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);    
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write(c);
  display.display();
 }

 void print_str(char* c)
 {

  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);     
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println(c);
  display.display();
  }

 void PrintQRCode(const char * data){
  QRCode qrcode;

  const int ps = 2; //pixels / square
  
  int QRCODE_VERSION = 3; 
  uint8_t qrcodeData[qrcode_getBufferSize(QRCODE_VERSION)];
  qrcode_initText(&qrcode, qrcodeData, QRCODE_VERSION, ECC_LOW, data);

  display.clearDisplay();
  int offset = 32;
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      //If pixel is on, we draw a ps x ps black square
      if(qrcode_getModule(&qrcode, x, y)){
        
        for(int xi = offset + x*ps; xi < offset + x*ps + ps; xi++){
          for(int yi= y*ps + 2; yi < y*ps + ps + 2; yi++){
            display.drawPixel(xi, yi, WHITE);
          }
        }
      }
    }
  }
  
   display.display();
}
