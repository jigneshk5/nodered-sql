#include <ESP8266WiFi.h>
#include <MQTT.h>
#include "DHT.h"

WiFiClient net;
MQTTClient client;
#define DHTTYPE DHT11   // DHT 11

const int DHTPin = 5;   //D1

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
String ssid="Tenda_10DF10";
String pass="jignesh1478";
unsigned long lastMillis = 0;


void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("nodemcu_client","user","password")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  dht.begin();
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("192.168.0.196", net);
  //client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(1000);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every 8 second.
  if (millis() - lastMillis > 8000) {
    lastMillis = millis();
    int ldr = analogRead(A0);   // read the ldr input on analog pin 0
    client.publish("nodemcu/ldr",String(ldr));
    
    float h = dht.readHumidity();
    float temp = dht.readTemperature();
    if (isnan(h) || isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    client.publish("nodemcu/dht","{\"temp\":"+String(temp)+", \"humidity\":"+String(h)+"}");

    Serial.println("Ldr: "+String(ldr)+"  Temp: "+String(temp)+" humidity: "+String(h));
  }
}
