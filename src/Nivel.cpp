#include <LM92.h>
#include <Wire.h>
//#include <ThingSpeak.h>

#include <ESP8266WiFi.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager

// const char* ssid     = "papaya";
// const char* password = "papaya2014";
// int status = WL_IDLE_STATUS;

#define vccPin D5 // Pino q alimenta o sensor
#define trigPin D6 // Trigger Pin
#define echoPin D7 // Echo Pin

long duration, distance; // Duration used to calculate distance

WiFiClient  client;

//emoncms
void sendToEmonCMS(String nodeId, String data1, String data2);
const char* emoncmsKey = "673d68c21abb07c581469836abc8aa97";
const char* host = "emoncms.org";


LM92 lm92(0,1);  //address 1, 0

//unsigned long myChannelNumber = 297575;
//const char * myWriteAPIKey = "95P28PYJT1I8PKUY";

uint32_t sleep_time_s = 30 * 60 * 1000000;  //30x60x1s = 30 min

void setup()
{
      // We start by connecting to a WiFi network
    Serial.begin(9600);
    //Wire.begin([SDA], [SCL]).    //Wire.begin([SDA], [SCL]).
    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP8266");
    Serial.println("connected...yeey :)");
    Wire.begin();

    lm92.enableFaultQueue(true); //optional, make readings more tolerent to inteference
    lm92.ResultInCelsius = true;  //change to Celsius mode

    pinMode(vccPin, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    //ThingSpeak.begin(client);

}

void loop()
{
    //temperatura
    float temperatura = lm92.readTemperature();
    String s_temp = String(temperatura,1);
    Serial.println(temperatura);


    //distancia
    digitalWrite(vccPin, HIGH);    //liga a alimentação
    delay(300);  //dar tempo ao sensor estabilizar
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    //Calculate the distance (in cm) based on the speed of sound.
    distance = duration/58.2;
    String s_distance = String(distance);
    digitalWrite(vccPin, LOW);
    Serial.println(distance);
    //Delay 50ms before next reading.
    delay(50);

    sendToEmonCMS("0", s_temp, s_distance);

    //ThingSpeak.writeField(myChannelNumber, 1, temperatura, myWriteAPIKey);
    //delay(10000);
    ESP.deepSleep(sleep_time_s);
}

void sendToEmonCMS(String nodeId, String data1, String data2) {
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/input/post.json?node=";
  url += nodeId;
  url += "&apikey=";
  url += emoncmsKey;
  url += "&csv=";
  url += data1;
  url += ",";
  url += data2;

  //Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
    //Serial.print(line);
  }
}
