
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <SPI.h>


byte ledPin = 2;
char ssid[] = "@Ahmad_tar";           // SSID of your home WiFi
char pass[] = "m7md2007";              // password of your home WiFi
WiFiServer server(80);                    

IPAddress ip(192, 168, 10, 120);            // IP address of the server
IPAddress gateway(192, 168, 10, 111);           // gateway of your network
IPAddress subnet(255,255,255,0);          // subnet mask of your network


unsigned long DHTtimer = 0;
float h, t;
unsigned long clientTimer = 0;


#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(115200);                     // only for debug
  dht.begin();

  WiFi.config(ip, gateway, subnet);       // forces to use the fix IP
  WiFi.begin(ssid, pass);                 // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  server.begin();                         // starts the server
/*  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());  // some parameters from the network
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  Serial.print("Networks: "); Serial.println(WiFi.scanNetworks());*/
  pinMode(ledPin, OUTPUT);                      // starts the server
}

void loop() {
  for(;;){
  getDHT();
    String temp = String(t);
    String hum = String(h);
 


  WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      digitalWrite(ledPin, LOW);  // to show the communication only (inverted logic)
      Serial.println(".");
      String request = client.readStringUntil('\r');    // receives the message from the client
      Serial.print("From client: "); Serial.println(request);
      client.flush();
      client.println(temp  + "/"+ hum + "\r"); // sends the answer to the client
      
      digitalWrite(ledPin, HIGH);
    }
    client.stop();                // tarminates the connection with the client
  }

  delay(1000);
  }
}


void getDHT()
{
  float tempIni = t;
  float humIni = h;
  t = dht.readTemperature();
  h = dht.readHumidity();
  if (isnan(h) || isnan(t))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    t = tempIni;
    h = humIni;
    return;
  }
}











