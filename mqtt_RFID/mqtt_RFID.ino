#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Network SSID and password
const char* ssid = "AssNet";
const char* password = "assemblybos";

// MQTT Broker Configuration
const char* mqtt_server = "10.16.1.6";  // Ganti dengan alamat IP broker MQTT Anda
const int mqtt_port = 1883;             // Port MQTT default
const char* mqtt_user = "ben";          // Username MQTT Anda
const char* mqtt_password = "1234";     // Password MQTT Anda

#define LED_PIN D0  // D8
#define BUZ_PIN D1  // D1

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char IDTAG[20];
char chipID[25];                       // Store ESP8266 Chip ID
char nodevice[20] = "202309A001";      // Change to your desired Node Device (max 20 characters)
char key[50] = "1234567890987654321";  // Change to your desired Key (max 20 characters)

MFRC522 mfrc522(SDA_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  pinMode(BUZ_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  buzz(0);

  // Setting up Wi-Fi connection
  WiFi.hostname("NodeMCU");
  WiFi.begin(ssid, password);

  // Check Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("Connected to Wi-Fi");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);
  Serial.println("Chip ID: ");
  Serial.println(chipID);

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Place your RFID Card on the Reader");
  buzz(3);

  // Setup MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Subscribe to a topic
  client.subscribe("responServer");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int berhasilBaca = bacaTag();

  if (berhasilBaca) {
    static char hasilTAG[20] = "";  // Store previous tag ID

    if (IDTAG) {
      buzz(1);
    } else {
      buzz(0);
    }

    if (strcmp(hasilTAG, IDTAG) != 0) {
      strcpy(hasilTAG, IDTAG);
      Serial.println("Card ID: " + String(IDTAG));

      // Send data to server and receive JSON response
      String jsonResponse = sendCardIdToServer(IDTAG);

      Serial.println("Selesai untuk ID: " + String(IDTAG));
      Serial.println("=======================");
      Serial.println("");
    } else {
      Serial.println("...");
    }

    delay(1000);
    strcpy(hasilTAG, "");
  } else {
    strcpy(IDTAG, "");
  }

  buzz(0);
}

int bacaTag() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return 0;

  if (!mfrc522.PICC_ReadCardSerial())
    return 0;

  memset(IDTAG, 0, sizeof(IDTAG));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    snprintf(IDTAG, sizeof(IDTAG), "%s%02X", IDTAG, mfrc522.uid.uidByte[i]);
  }

  return 1;
}

String sendCardIdToServer(String cardId) {
  String jsonResponse = "";
  // Send RFID card data, Chip ID, Node Device, and Key to the server
  String request = "{";
  request += "\"nokartu\":\"" + String(cardId) + "\",";
  request += "\"idchip\":\"" + String(chipID) + "\",";
  request += "\"nodevice\":\"" + String(nodevice) + "\",";
  request += "\"key\":\"" + String(key) + "\"";
  request += "}";

  if (client.connect("NodeMCU_Client", mqtt_user, mqtt_password)) {
    Serial.println("Konek ke MQTT Broker");
    Serial.println("Kirim ke topik dariMCU: " + request);
    client.publish("dariMCU", request.c_str());
    // client.disconnect();
  } else {
    Serial.println("Koneksi ke MQTT Broker gagal");
  }

  return jsonResponse;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Received message on topic: " + String(topic));
  Serial.println("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop sampai terhubung ke broker MQTT
  while (!client.connected()) {
    Serial.print("Connecting to MQTT Broker...");
    // Coba terhubung ke broker MQTT
    if (client.connect("NodeMCU_Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Langganan topik yang Anda butuhkan di sini jika diperlukan
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retry in 5 seconds");
      // Tunggu sebelum mencoba lagi
      delay(5000);
    }
  }
}

void buzz(int loop) {
  if (loop == 0) {
    digitalWrite(BUZ_PIN, LOW);
  } else if (loop == 1) {
    digitalWrite(BUZ_PIN, HIGH);
  } else {
    for (int t = 0; t < loop; t++) {
      digitalWrite(BUZ_PIN, HIGH);
      delay(200);
      digitalWrite(BUZ_PIN, LOW);
      delay(200);
    }
  }
}
