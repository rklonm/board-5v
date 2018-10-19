#include "env.h"

#include "DHTesp.h"

#ifndef ESP32
#pragma message(THIS CODE FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif


#include <WiFi.h>

const char* ssid     = envssid;
const char* password = envkey;


DHTesp dht;
const int dhtPin = 16;

WiFiServer server(80);


#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include <Wire.h>
#include <Adafruit_BMP085.h>

#include "uRTCLib.h"
uRTCLib rtc(0x68, 0x57);

uint64_t chipid;  

#define LED_BUILTIN 2


#define tftclk 18
#define tftmosi 23
#define tftmiso 19
#define tftcs   15// 21
#define tftrst  4 // 22
#define tftdc   17
//#define irpin 4

Adafruit_ILI9341 tft = Adafruit_ILI9341(tftcs, tftdc, tftmosi, tftclk, tftrst, tftmiso);
Adafruit_BMP085 bmp;   
long Temperature = 0, Pressure = 0;

IPAddress myip;
TempAndHumidity lastValues;

void setup() {
  digitalWrite(LED_BUILTIN, HIGH);  
  pinMode(LED_BUILTIN, OUTPUT);  
  Serial.begin(115200);
  dht.setup(dhtPin, DHTesp::DHT11);

  Wire.begin();
  bmp.begin();
//  scani2c();

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
//    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
//        Serial.write(c);                    // print it out the serial monitor
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
            client.print("<a href=\"/G\">GET</a><br>");

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
        if (currentLine.endsWith("GET /G")) {
          collectData();
        }
        
      }
    }
    // close the connection:
    client.stop();
//    Serial.println("Client Disconnected.");
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

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void collectData(){
  TempAndHumidity lastValues = dht.getTempAndHumidity();
  Serial.println("Temperature: " + String(lastValues.temperature,0));
  Serial.println("Humidity: " + String(lastValues.humidity,0));



    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");
    
    Serial.println();


/*
  dps.getPressure(&Pressure); 
  dps.getTemperature(&Temperature);
 
  Serial.print("Pressure(mm Hg):"); 
  Serial.print(Pressure/133.3);      //Выводим давление в мм.рт ст
  Serial.print("Temp:"); 
  Serial.println(Temperature*0.1);   // Температуру в градусах Цельсия
  */


 // rtc.set(0, 42, 16, 6, 2, 5, 15);
  //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)

  rtc.refresh();


  Serial.print("RTC DateTime: ");
  Serial.print(rtc.year());
  Serial.print('/');
  Serial.print(rtc.month());
  Serial.print('/');
  Serial.print(rtc.day());

  Serial.print(' ');

  Serial.print(rtc.hour());
  Serial.print(':');
  Serial.print(rtc.minute());
  Serial.print(':');
  Serial.print(rtc.second());

  Serial.print(" DOW: ");
  Serial.print(rtc.dayOfWeek());

  Serial.print(" - Temp: ");
  Serial.print(rtc.temp());

  Serial.println();




  
}

void scani2c(){
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");  
}
