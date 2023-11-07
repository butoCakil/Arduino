#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "AssNet";
const char* password = "assemblybos";

// MQTT broker
const char* mqtt_server = "10.16.1.6";
const char* mqtt_username = "ben";
const char* mqtt_password = "1234";

int hitung;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("NodeMCUClient", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.println("Connection failed. Retrying...");
      delay(2000);
    }
  }

  // Subscribe to a topic
  client.subscribe("responServer");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void loop() {
  // Publish a message
  client.publish("dariMCU", "Hello from NodeMCU");

  Serial.print("kirim : ke "); 
  Serial.println(hitung);
  // Handle MQTT messages
  client.loop();

  delay(5000);
  hitung++;
}
