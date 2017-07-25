#include <LM92.h>
#include <Wire.h>
#include <ThingSpeak.h>

#include <ESP8266WiFi.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager
#include <Ticker.h>
#include <NewPing.h>

//sensor temperatura
#define SDA D1
#define SCL D2

//sensor distancia
#define vccPin       D5 // Pino q alimenta o sensor
#define TRIGGER_PIN  D6  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

long duration; // Duration used to calculate distance
String distance;

WiFiClient  client;

// //emoncms
// void sendToEmonCMS(String nodeId, String data1, String data2);
// const char* emoncmsKey = "673d68c21abb07c581469836abc8aa97";
// const char* host = "emoncms.org";

//for LED status
Ticker ticker;
void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

LM92 lm92(0,1);  //address 1, 0

//unsigned long myChannelNumber = 297575;
//const char * myWriteAPIKey = "95P28PYJT1I8PKUY";

uint32_t sleep_time_s = 30 * 60 * 1000000;  //30x60x1s = 30 min

//Entra nesta função quando o Wifimamager está em modo de configuração
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setup()
{
      // We start by connecting to a WiFi network
    Serial.begin(9600);
    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP8266", "smolder"); //nome e senha para acessar o portal
    Serial.println("connected...yeey :)");
    ticker.detach();
    Wire.begin(SDA, SCL);

    lm92.enableFaultQueue(true); //optional, make readings more tolerent to inteference
    lm92.ResultInCelsius = true;  //change to Celsius mode

    pinMode(vccPin, OUTPUT);
    digitalWrite(vccPin, HIGH);    //liga a alimentação

    ThingSpeak.begin(client);
}

void loop()
{
    //temperatura
    float temperatura = lm92.readTemperature();
    String s_temp = String(temperatura,1);
    Serial.println(temperatura);

    double_t echoTime = sonar.ping_median(5);  //media de medicoes
    distance = sonar.convert_cm(echoTime);
    Serial.print("Ping: ");
    Serial.print(distance); // Send ping, get distance in cm and print result (0 = outside set distance range)
    Serial.println("cm");

    sendToEmonCMS("0", s_temp, distance);

    ThingSpeak.writeField(myChannelNumber, 1, temperatura, myWriteAPIKey);
    delay(3000);
    ESP.deepSleep(sleep_time_s);
}

  // void sendToEmonCMS(String nodeId, String data1, String data2) {
  //   const int httpPort = 80;
  //   if (!client.connect(host, httpPort)) {
  //     Serial.println("connection failed");
  //     return;
  //   }

  // // We now create a URI for the request
  // String url = "/input/post.json?node=";
  // url += nodeId;
  // url += "&apikey=";
  // url += emoncmsKey;
  // url += "&csv=";
  // url += data1;
  // url += ",";
  // url += data2;

  //Serial.println(url);

  // This will send the request to the server
  // client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  //              "Host: " + host + "\r\n" +
  //              "Connection: close\r\n\r\n");
  // delay(10);
  //
  // // Read all the lines of the reply from server
  // while (client.available()) {
  //   String line = client.readStringUntil('\r');
  //   //Serial.print(line);
  // }
}
