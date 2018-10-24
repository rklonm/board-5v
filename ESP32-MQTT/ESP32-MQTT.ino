#include "env.h"
#include "cyrillic.h"

#ifndef ESP32
#pragma message(THIS CODE FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif


#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "uRTCLib.h"
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"



const char* wifissid = envwifissid;
const char* wifipass = envwifipass;
const char* mqttuser = envmqttuser;
const char* mqttpass = envmqttpass;
const char* mqttsrvr = envmqttsrvr;
const   int mqttport = envmqttport;
const char* mqttdevice = envmqttdevice;

const int tftcs   = envtftcs;
const int tftdc   = envtftdc;
const int tftmosi = envtftmosi;
const int tftclk  = envtftclk;
const int tftrst  = envtftrst;
const int tftmiso = envtftmiso;

const int dhtpin = envdhtpin;



extern "C" {      
   uint8_t temprature_sens_read(); 
   uint32_t hall_sens_read();
}


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char tmpmsg[50];
int value = 0;

bool is_boot=false;
DHTesp dht;
Adafruit_BMP085 bmp;
uRTCLib rtc(0x68, 0x57);
Adafruit_ILI9341 tft = Adafruit_ILI9341(tftcs, tftdc, tftmosi, tftclk, tftrst, tftmiso);

#include "ui.h"
UI ui;


void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  Serial.begin(115200);
  Wire.begin();
  bmp.begin(3);  
  tft.begin();

  ui.begin();
  
  dht.setup(dhtpin, DHTesp::DHT11);
  setup_wifi();
  ui.show("boot","mqtt server ...");
  client.setServer(mqttsrvr, mqttport);
  ui.show("boot","mqtt callback ...");
  client.setCallback(callback);
  ui.show("boot","Загрузка завершена");
  reconnect();
  is_boot=false;
}

void setup_wifi() {
  ui.show("boot","wifi starting ...");
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifissid);

  WiFi.begin(wifissid, wifipass);

  int tmp=0;
  while (WiFi.status() != WL_CONNECTED) {
    tmp++;
    ui.show("boot","wifi connect "+ String(tmp));
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  int tmp=0;
  while (!client.connected()) {
    tmp++;
    if(is_boot){
      ui.show("boot","mqtt connect "+String(tmp));
    }

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttdevice,mqttuser,mqttpass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
  tmp=0;
  if(is_boot){
    ui.show("boot","mqtt connected");
  }
}

void sendfloat(char* channel, float val){
    snprintf (tmpmsg, 50, "%f", val);
    Serial.print(channel);
    Serial.print(" : ");
    Serial.println(tmpmsg);
    client.publish(channel, tmpmsg);
}

void sendint(char* channel, int val){
    snprintf (tmpmsg, 50, "%d", val);
    Serial.print(channel);
    Serial.print(" : ");
    Serial.println(tmpmsg);
    client.publish(channel, tmpmsg);
}

void loop() {
  digitalWrite(LED_BUILTIN,HIGH);  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

    rtc.refresh();
    
    float dht_temp=dht.getTemperature();
    float dht_humm=dht.getHumidity();
    
    float bmp_temp=bmp.readTemperature();
    float bmp_pres=bmp.readPressure()/133.322;
    
    float esp_temp=( temprature_sens_read() - 32 )/1.8;
    float rtc_temp=rtc.temp();


    ui.show("A",bmp_pres);
    ui.show("B",dht_humm);
//    ui.show("C"," ");
//    ui.show("D"," ");
    ui.show("E",bmp_temp);
    ui.show("F",dht_temp);
    ui.show("G",rtc_temp);
    ui.show("H",esp_temp);

    
    sendfloat("/b5v/dht11/temperature",dht_temp);
    sendfloat("/b5v/dht11/humidity",dht_humm);
    sendfloat("/b5v/bmp180/temperature",bmp_temp);        
    sendfloat("/b5v/bmp180/pressure",bmp_pres);
    sendfloat("/b5v/esp32/temperature",esp_temp);
    sendfloat("/b5v/rtc/temperature",rtc_temp);

  }
}
