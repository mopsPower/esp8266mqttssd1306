#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "public.mqtthq.com";

// SSD1306Wire display(0x3c, SDA, SCL);
SSD1306Wire display(0x3c, 2, 0, GEOMETRY_128_64);
WiFiManager wm;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String s;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    s+=(char)payload[i];
  }
  display.clear();
  display.drawString(10, 10, s);
  display.display();
  // Serial.println();

  // Switch on the LED if an 1 was received as first character
  // if ((char)payload[0] == '1') {
  //   digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  // } else {
  //   digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  // }
}

void setup()
{
  Serial.begin(9600);
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_16);

  WiFi.mode(WIFI_STA);
  boolean res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  display.drawString(10, 10, "conn...");
  display.display();
  res=wm.autoConnect("config", "codetyphon"); // password protected ap
  if(!res){
    display.clear();
    display.drawString(10, 10, "config");
    display.display();
  }
  display.clear();
  display.drawString(10, 10, WiFi.localIP().toString());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe("codetyphon/test");
  display.display();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("codetyphon/", "hello world");
      // ... and resubscribe
      client.subscribe("codetyphon/test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}