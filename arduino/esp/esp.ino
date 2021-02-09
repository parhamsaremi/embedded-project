#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <string>

const char* ssid = "Fereshteh";
const char* password = "rze281re";
const char* host = "http://embedded-app-llama.herokuapp.com/";
String inputString = "";         // a String to hold incoming data


void setup () {

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting..");
  }

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    while (Serial.available()) {
     // get the new byte:
     char inChar = (char)Serial.read();
     // add it to the inputString:
     if (inChar != '\n') 
      inputString += inChar;
     // if the incoming character is a newline, set a flag so the main loop can
     // do something about it:
     if (inChar == '\n') 
       break;
    }
   
    if (inputString == "GET_QR") {
      HTTPClient http_QR;
      http_QR.begin("http://embedded-app-llama.herokuapp.com/get_code");
      int httpCode = http_QR.GET();     //Send the request                                                           

      if (httpCode > 0) { //Check the returning code
        String payload = http_QR.getString();   //Get the request response payload
        Serial.println(payload);             //Print the response payload
      }
      inputString = "";
      http_QR.end();   //Close connection 
    }

    if (inputString == "GET_LOCK") {
      HTTPClient http_lock;
      http_lock.begin("http://embedded-app-llama.herokuapp.com/get_lock");
      int httpCode = http_lock.GET();     //Send the request                                                           

      if (httpCode > 0) { //Check the returning code
        String payload = http_lock.getString();   //Get the request response payload
        Serial.println(payload);             //Print the response payload
      }
      inputString = "";
      http_lock.end();   //Close connection 
    }
  }


  delay(500);    //Send a request every 0.5 seconds

}
