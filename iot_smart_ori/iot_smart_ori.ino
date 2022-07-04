#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h> 
#include <Fonts/FreeSerif9pt7b.h> 
#include <Fonts/FreeMono9pt7b.h>  //custom font

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1 
 
#define DHTPIN D4          //22
DHT dht(DHTPIN, DHT22);
 
String apiKey = "13KF7N63BMTCOISN"; 
const char *ssid = "SLT FIBER";     
const char *pass = "0112722247";
const char* server = "api.thingspeak.com";
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
const int AirValue = 790;   
const int WaterValue = 390;  
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;
int rpin = D5;

WiFiClient client;
 
 
void setup() {
  Serial.begin(9600); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  pinMode(rpin, OUTPUT);
 
  dht.begin();
  
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
    Serial.println("WiFi connected");
    delay(4000);
}  
 
 
void loop() 
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  Serial.print("Humidity: ");
  Serial.print(h,0);
  Serial.println("%");
  Serial.print("Temperature: ");
  Serial.print(t,0);
  Serial.println("%");
  delay(100);
  soilMoistureValue = analogRead(SensorPin);  
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
 
 
if(soilmoisturepercent >= 100)
{
  Serial.print("soilmoisture: ");
  Serial.println("100%");
  
  display.setCursor(0,20);  //oled display
  display.setTextSize(0);
  display.setFont(&FreeMono9pt7b);  // ac custom font
  display.setTextColor(WHITE);
  display.print("Moist:");
  display.setTextSize(0);
  display.print("100");
 display.println("%");
  display.setCursor(0,40);  //oled 
  display.setTextSize(0);
  display.print("Humid:");
  display.setTextSize(0);
  display.print(h,0);
  display.println("%");
  display.setCursor(0,60);  //oled 
  display.setTextSize(0);
  display.print("Temp:");
  display.setTextSize(0);
  display.print(t,0);
  display.println("*C");
  display.display();
  
  delay(100);
  display.clearDisplay();
}
 
 
else if(soilmoisturepercent <=0)
{
  Serial.print("soilmoisture: ");
  Serial.println("0%");
  
  display.setCursor(0,20);  //oled display
  display.setTextSize(0);
  display.setFont(&FreeMono9pt7b);  // Set a custom font
  display.setTextColor(WHITE);
  display.print("Moist:");
  display.setTextSize(0);
  display.print("0");
  display.println("%");
  display.setCursor(0,40);  //oled display
  display.setTextSize(0);
  display.print("Humid:");
  display.setTextSize(0);
  display.print(h,0);
  display.println("%");
  display.setCursor(0,60);  //oled display
  display.setTextSize(0);
  display.print("Temp:");
  display.setTextSize(0);
  display.print(t,0);
  display.println("*C");
  display.display();
 
  delay(100);
  display.clearDisplay();
}
 
 
else if(soilmoisturepercent >=0 && soilmoisturepercent <= 100)
{
  Serial.print("soilmoisture : ");
  Serial.print(soilmoisturepercent);
  Serial.println("%");
  
  display.setCursor(0,20);  //oled display
  display.setTextSize(0);
  display.setFont(&FreeMono9pt7b);  // Set a custom font
  display.setTextColor(WHITE);
  display.print("Moist:");
  display.setTextSize(0);
  display.print(soilmoisturepercent);
  display.println("%");
  display.setCursor(0,40);  //oled display
  display.setTextSize(0);
  display.print("Humid:");
  display.setTextSize(0);
  display.print(h,0);
  display.println("%");
  display.setCursor(0,60);  //oled display
  display.setTextSize(0);
  display.print("Temp:");
  display.setTextSize(0);
  display.print(t,0);
  display.println("*C");
  display.display();
 
  delay(100);
  display.clearDisplay();
}
 
  if(soilmoisturepercent <= 49)
  {
    digitalWrite(rpin, LOW);
    Serial.println("Motor is ON");
   Serial.println("-------------------");
  }
  else if (soilmoisturepercent >49 && soilmoisturepercent <= 100)
  {
    digitalWrite(rpin, HIGH);
    Serial.println("Motor is OFF");
   Serial.println("-------------------");
  }
  
  if (client.connect(server, 80))
  {
    String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(soilmoisturepercent);
      postStr += "&field2=";
      postStr += String(h);
      postStr += "&field3=";
      postStr += String(t);
      postStr += "&field4=";
      postStr += String(rpin);
      postStr += "\r\n\r\n\r\n\r\n";
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
   
  }
    client.stop();
}
