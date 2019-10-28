#include <DHT.h>

#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include "images.h"
byte ledPin = 2;
char ssid[] = "@Ahmad_tar";           // SSID of your home WiFi
char pass[] = "m7md2007";               // password of your home WiFi

IPAddress server(192, 168, 10, 120);  // fix IP of the server
WiFiClient client;


ThreeWire myWire(D6, D5, D7); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))

const char data[] = "what time is it";

/* Hello World OLED Test */

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
SSD1306  display(0x3c, 5, 4); // Initialize the OLED display using Wire library

/* DHT22 */
#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float localHum = 0;
float localTemp = 0;


int draw_state = 0;
int counter = 1;

String request1, request2;

void setup()
{
  Serial.begin(115200);


  WiFi.begin(ssid, pass);             // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  /*  Serial.println("Connected to wifi");
    Serial.print("Status: "); Serial.println(WiFi.status());    // Network parameters
    Serial.print("IP: ");     Serial.println(WiFi.localIP());
    Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
    Serial.print("SSID: "); Serial.println(WiFi.SSID());
    Serial.print("Signal: "); Serial.println(WiFi.RSSI());*/
  pinMode(ledPin, OUTPUT);

  display.init(); // Initialising the UI will init the display too.

  display.flipScreenVertically();
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (Rtc.GetIsWriteProtected())
  {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }


  /* comment out on a second run to see that the info is stored long term */
  // Store something in memory on the RTC
  uint8_t count = sizeof(data);
  uint8_t written = Rtc.SetMemory((const uint8_t*)data, count); // this includes a null terminator for the string
  if (written != count)
  {
    Serial.print("something didn't match, count = ");
    Serial.print(count, DEC);
    Serial.print(", written = ");
    Serial.print(written, DEC);
    Serial.println();
  }







}

void loop()
{



    RtcDateTime now = Rtc.GetDateTime();
  
  
    Serial.println("\n");
  
    delay(5000);
  
    uint8_t buff[20];
    const uint8_t count = sizeof(buff);
    // get our data
    uint8_t gotten = Rtc.GetMemory(buff, count);
  
    if (gotten != count)
    {
      Serial.print("something didn't match, count = ");
      Serial.print(count, DEC);
      Serial.print(", gotten = ");
      Serial.print(gotten, DEC);
      Serial.println();
    }
  
    Serial.print("data read (");
    Serial.print(gotten);
    Serial.print(") = \"");
    // print the string, but terminate if we get a null
    for (uint8_t ch = 0; ch < gotten && buff[ch]; ch++)
    {
      Serial.print((char)buff[ch]);
    }
    Serial.println("\"");
  
    delay(5000);

  

  
  
  for (;;) {
    drawProgressBarDemo();
    client.connect(server, 80);   // Connection to the server
  digitalWrite(ledPin, LOW);    // to show the communication only (inverted logic)
  Serial.println(".");
  client.println("Hello server! Are you sleeping?\r");  // sends the message to the server
String answer = client.readStringUntil('\r');   // receives the answer from the sever
  Serial.println("from server: " + answer);
  client.flush();
  digitalWrite(ledPin, HIGH);
  delay(2000);

  request1 = answer.substring(0, 5);
  request2 = answer.substring(6, 11);
  Serial.println(request1);
  Serial.println(request2);

    switch (draw_state ) {
display.clear();
display.display();
      case 0: printDateTime(now); delay (3000); break;
      
      case 1: drawInTemperature(); delay (3000); break;
      case 2: drawInHumidity(); delay (3000); break;
      case 3: drawOutTemperature(request1); delay (3000); break;
      case 4: drawOutHumidity(request2); delay (3000); break;
    }

    draw_state++;
    if (draw_state > 4) {
      draw_state = 0;
    }
  }




}


void drawProgressBarDemo() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

/***************************************************
  Get indoor Temp/Hum data
****************************************************/
void getDHT()
{
  float tempIni = localTemp;
  float humIni = localHum;
  localTemp = dht.readTemperature();
  localHum = dht.readHumidity();
  if (isnan(localHum) || isnan(localTemp))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    localTemp = tempIni;
    localHum = humIni;
    return;
  }
}

/***************************************************
  Draw Indoor Page
****************************************************/
void drawInTemperature()
{
  int x = 0;
  int y = 0;

  getDHT();
  display.clear();



  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(35 + x, y, "INDOOR");



  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(45 + x, 20 + y, "Temp");

  display.setFont(ArialMT_Plain_16);
  String temp = String(localTemp) + "°C";
  display.drawString(37 + x, 40 + y, temp);
  int tempWidth = display.getStringWidth(temp);

  display.display();
  Serial.println("in: " + temp);
}



void drawOutTemperature(String t)
{
  int x = 0;
  int y = 0;


  display.clear();



  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(30 + x, y, "OUTDOOR");



  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(45 + x, 20 + y, "Temp");

  display.setFont(ArialMT_Plain_16);
  String temp2 = t + "°C";
  display.drawString(37 + x, 40 + y, temp2);
  int tempWidth = display.getStringWidth(temp2);

  display.display();

}



void drawInHumidity()
{
  int x = 0;
  int y = 0;
  getDHT();
  display.clear();

  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(35 + x, y, "INDOOR");

  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(45 + x, 20 + y, "Hum");



  display.setFont(ArialMT_Plain_16);
  String hum = String(localHum) + "%";
  display.drawString(37 + x, 40 + y, hum);
  int humWidth = display.getStringWidth(hum);

  display.display();
}

void drawOutHumidity(String h)
{
  int x = 0;
  int y = 0;
  getDHT();
  display.clear();

  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(30 + x, y, "OUTDOOR");

  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(45 + x, 20 + y, "Hum");



  display.setFont(ArialMT_Plain_16);
  String hum2 = h + "%";
  display.drawString(37 + x, 40 + y, hum2);
  int humWidth = display.getStringWidth(hum2);

  display.display();
}


void printDateTime(const RtcDateTime& dt)
{
  display.clear();
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u\n%02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_24);
  String tim = (String)datestring;
  
  display.drawString(15, 15, tim.substring(11));
  display.display();
}


