#include "env.h"

#include <WiFi.h>



const char* ssid     = envssid;
const char* password = envkey;

WiFiServer server(80);


#include <SPI.h>
#include <SSD_13XX.h> //https://github.com/sumotoy/SSD_13XX

uint64_t chipid;  

#define LED_BUILTIN 2
#define LED_TEST 16

#define oledsclk 18
#define oledmosi 23
#define oledcs   21
#define oledrst  22
#define oleddc   17
//#define irpin 4

// определение цветов
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

//SSD_13XX display = SSD_13XX(oledcs, oleddc, oledmosi, oledsclk, oledrst);
SSD_13XX tft = SSD_13XX(oledcs, oleddc, oledrst);

//Adafruit_SSD1331 display = Adafruit_SSD1331(oledcs, oleddc, oledmosi, oledsclk, oledrst);


void setup() {
  digitalWrite(LED_BUILTIN, HIGH);  
  pinMode(LED_BUILTIN, OUTPUT);  
	Serial.begin(115200);
  tft.begin();
  tft.fillScreen(RED);
  
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
        tft.fillScreen(BLUE);
        delay(3000);
        tft.fillScreen(RED);
        delay(3000);
        Serial.print(".");
  }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    ps(WiFi.localIP());
    server.begin();
    
    pinMode(LED_TEST, OUTPUT);
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
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
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
          digitalWrite(5, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

  
//	chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
//	Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes
//	Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
//  digitalWrite(LED_TEST, HIGH);   // turn the LED on (HIGH is the voltage level)
//  benchmark();
//  delay(1000);                       // wait for a second

}



void ps(IPAddress text){
  tft.clearScreen();
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextScale(1);
  tft.println(text);
  
}


