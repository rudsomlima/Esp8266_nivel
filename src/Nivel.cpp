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
WiFiClient  client;

//emoncms
void sendToEmonCMS(String nodeId, String data);
const char* emoncmsKey = "673d68c21abb07c581469836abc8aa97";
const char* host = "emoncms.org";


LM92 lm92(0,1);  //address 1, 0

//unsigned long myChannelNumber = 297575;
//const char * myWriteAPIKey = "95P28PYJT1I8PKUY";

uint32_t sleep_time_s = 2 * 60 * 1000000;

void setup()
{
      // We start by connecting to a WiFi network
    Serial.begin(9600);
    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP8266");
    Serial.println("connected...yeey :)");
    Wire.begin();

    lm92.enableFaultQueue(true); //optional, make readings more tolerent to inteference
    lm92.ResultInCelsius = true;  //change to Celsius mode
    //ThingSpeak.begin(client);

}

void loop()
{
    float temperatura = lm92.readTemperature();
    String s_temp = String(temperatura,1);
    Serial.println(temperatura);
    sendToEmonCMS("0", s_temp);

    //ThingSpeak.writeField(myChannelNumber, 1, temperatura, myWriteAPIKey);
    delay(10000);
    //ESP.deepSleep(sleep_time_s, WAKE_RF_DEFAULT);
}

void sendToEmonCMS(String nodeId, String data) {
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
  url += data;

  Serial.println(url);

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
