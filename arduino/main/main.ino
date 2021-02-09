

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <qrcode.h>
#include <SoftwareSerial.h>

/* LCD */

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(-1);

SoftwareSerial espSerial(10, 11); // rx, tx



/* KEYPAD */
const byte ROWS = 4, COLS = 4;
String inputString = "";         // a String to hold incoming data
String lock = "false";
int times = 0;

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
#define WRONG_USER 4

unsigned char state;


void print_char(char c);
void print_str(char* c);
void PrintQRCode(const char *url);

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(100);

  state = WAIT;
  
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);

  digitalWrite(relay_1, LOW);
  digitalWrite(relay_2, LOW);

}

void loop() {

  switch (state) {
    case WAIT: {
      // wait for user to press key
      
      print_str("enter 'A' to get QR code");
      Serial.println("WAIT");
      char customKey = customKeypad.getKey();

      if(customKey) {
        print_char(customKey);
        if (customKey == 'A')
           state = GET_QR;
      }
      break;
    }
    
    case GET_QR: {
      //ask server for a qr code and wait for server response 
      //then go to show_qr state
      
      print_str("please wait...");
      inputString = "";
      
      espSerial.print("GET_QR\n");
      Serial.println("GET_QR");
      while (!espSerial.available());
      
      while (espSerial.available()) {
       // get the new byte:
       char inChar = (char)espSerial.read();
       // add it to the inputString:
       if (inChar != '\n') 
        inputString += inChar;
       if (inChar == '\n') 
         break;
      }
      
      Serial.println(inputString);
      state = SHOW_QR;
      break;
    }
    
    case SHOW_QR: {
      //show the qr
      //then wait for server to send response
      //wait for xx seconds 
      //if response was go to state open door
      //else go to wait
      Serial.println("SHOW_QR");
      const char *str = inputString.c_str();
      PrintQRCode(str);

      lock = "false";
      times = 0;
      while (true) {
        espSerial.print("GET_LOCK\n");
        Serial.println("GET_LOCK");
        
        while (!espSerial.available());
        lock = "";
        while (espSerial.available()) {
         // get the new byte:
         char inChar = (char)espSerial.read();
         // add it to the inputString:
         if (inChar != '\n') 
          lock += inChar;

         if (inChar == '\n') {
           lock += '\0';
           break;
         }
        }

        Serial.println(lock);
        const char *lock_const = lock.c_str();
        
        if (strcmp(lock_const, "true") >= 0) {
          state = OPEN_DOOR;
          display.invertDisplay(false);
          break;
        }
        
        times += 1;

        if (times >= 20) {
          state = WRONG_USER;
          display.invertDisplay(false);
          break;
        }
        
        delay(2000);
      }
      break;
    }
    
    case OPEN_DOOR: {
      //open the door, wait for xx seconds, close the door
      
      print_str("opening...");
      Serial.println("OPEN_DOOR");
      digitalWrite(relay_1, HIGH);
      delay(1000);
      digitalWrite(relay_1, LOW);
      delay(1000);

      delay(2000);
      
      Serial.println("CLOSE_DOOR");
      print_str("closing...");
      digitalWrite(relay_2, HIGH);
      delay(1000);
      digitalWrite(relay_2, LOW);
      delay(1000);
      
      delay(2000);
  
      state = WAIT;
      break;
    }

    case WRONG_USER: {
      print_str("wrong user; restarting...");
      Serial.println("WRONG_USER");
      delay(2000);
      state = WAIT;
      break;
    }
    
    default: {
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

 void PrintQRCode(const char * data){
  QRCode qrcode;

  const int ps = 1; //pixels / square
  int QRCODE_VERSION = 3;
  uint8_t qrcodeData[qrcode_getBufferSize(QRCODE_VERSION)];
  qrcode_initText(&qrcode, qrcodeData, QRCODE_VERSION, ECC_LOW, data);

  display.clearDisplay();
  int offset = 32;
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      //If pixel is on, we draw a ps x ps black square
      if(qrcode_getModule(&qrcode, x, y)){
        for(int xi = offset + x*ps*2; xi < offset + x*ps*2 + ps*2; xi++){
          for(int yi= y*ps + 2; yi < y*ps + ps + 2; yi++){
            display.drawPixel(xi, yi, WHITE);
          }
        }
      }
    }
  }
  display.invertDisplay(true);
  display.display();
}
