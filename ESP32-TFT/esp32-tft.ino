#include "env.h"

#include <WiFi.h>

const char* ssid     = envssid;
const char* password = envkey;

WiFiServer server(80);


#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"


uint64_t chipid;  

#define LED_BUILTIN 2


#define tftclk 18
#define tftmosi 23
#define tftmiso 19
#define tftcs   21
#define tftrst  22
#define tftdc   17
//#define irpin 4

Adafruit_ILI9341 tft = Adafruit_ILI9341(tftcs, tftdc, tftmosi, tftclk, tftrst, tftmiso);

IPAddress myip;

void setup() {
  digitalWrite(LED_BUILTIN, HIGH);  
  pinMode(LED_BUILTIN, OUTPUT);  
  Serial.begin(115200);

  tft.begin();
  displayPrepare();
  myip=IPAddress(127,0,0,1);
  
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_BUILTIN, LOW); 
        delay(200);
        digitalWrite(LED_BUILTIN, HIGH);                 
        tft.fillScreen(ILI9341_RED);
        delay(3000);
        tft.fillScreen(ILI9341_GREEN);
        delay(3000);
        Serial.print(".");
        displayPrepare();
  }

  myip=WiFi.localIP();
  serialMyIp();
  displayMyIp();
  
  server.begin();
}

int value = 0;

void loop() {

WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<a href=\"/H\">ON</a><br>");
            client.print("<a href=\"/L\">OFF</a><br>");

            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          pinOn();
        }
        if (currentLine.endsWith("GET /L")) {
          pinOff();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

}

void pinOn(){
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("pinOn");
  displayPinOn();
}

void pinOff(){
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("pinOff");
  displayPinOff();
}

void displayPinPrepare(){
  tft.fillRect(10, 70, 220, 40, ILI9341_YELLOW);
  tft.drawRect(10, 70, 220, 40, ILI9341_BLACK);
  tft.setCursor(50, 70);
  tft.setTextSize(4);
}

void displayPinOn(){
  displayPinPrepare();
  tft.setTextColor(ILI9341_BLUE);
  tft.println("pinOn ");
}

void displayPinOff(){
  displayPinPrepare();
  tft.setTextColor(ILI9341_RED);
  tft.println("pinOff");
}

void displayMyIp(){
  tft.setTextSize(3);
  tft.println(myip);
}

void serialMyIp(){
  Serial.println(myip);  
}

void displayPrepare(){
  tft.setCursor(20, 5);
  tft.fillScreen(ILI9341_YELLOW);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(4);
  tft.println("board-5v");
  
}
