#include <ESP8266WiFi.h>
#include "DHT.h"

// Network ID
const char* ssid     = "ESEpro";
const char* password = "sijitekanwolu";
const char* host = "192.168.0.117";
const int port = 80;

// Deklarasi DHT
#define DHTPIN D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  // NodeMCU Utility
  Serial.begin(9600);
  dht.begin();

  // Networking
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  WiFiClient client;
  
  if (!client.connect(host, port)) {
    Serial.println("Connection failed");
    return;
  }

  // DHT get temp dan humid
  int temp = dht.readTemperature(false);
  int humid = dht.readHumidity();

  // nodemcuphp/index.php?mode=save&temperature=${temp}&humidity=${humid}
  String apiUrl = "/nodemcuphp/index.php?";
  apiUrl += "mode=save";
  apiUrl += "&temperature="+String(temp);
  apiUrl += "&humidity="+String(humid);

  Serial.println("=====================================================");
  Serial.println("\nSuhu: ");
  Serial.println(temp);
  Serial.println("\nKelembapan: ");
  Serial.println(humid);
  Serial.println("\n=====================================================");
  Serial.println("\n\n");
  
  // Set header Request
  client.print(String("GET ") + apiUrl + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // Pastikan tidak berlarut-larut
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 3000) {
      Serial.println(">>> Client Timeout !");
      Serial.println(">>> Operation failed !");
      client.stop();
      return;
    }
  }

  // Baca hasil balasan dari PHP
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
}
