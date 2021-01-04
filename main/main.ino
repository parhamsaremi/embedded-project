#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal.h>

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


LiquidCrystal lcd(A0,A1,A2,A3,A4,A5);


enum states {
  WAIT,
  GET_QR,
  SHOW_QR,
  OPEN_DOOR,
};

states state = WAIT;


void setup(){
  lcd.begin(16,2); 
}

void loop(){
  // char customKey = customKeypad.getKey();
  // if (customKey){
  //   lcd.clear();
  //   lcd.setCursor(0, 0); 
  //   lcd.print(customKey);
  // }

  switch (state)
  {
  case WAIT:
    char customKey = customKeypad.getKey();
    if(customKey == 'A')
    {
      state = GET_QR;
    }
    break;
  
  case GET_QR:
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Please wait..."); 
    //TODO: ask server for a qr code and wait for server response 
    //then go to show_qr state
    
    break;

  case SHOW_QR:
    //TODO: show the qr
    //then wait for server to send response
    //wait for xx seconds 
    //if response was go to state open door
    //else go to idle (and print message on lcd)
    break;

  case OPEN_DOOR:
    //TODO: open the door, wait for xx seconds, close the door
    state = WAIT;
    break;

  default:
    break;
  }
}
