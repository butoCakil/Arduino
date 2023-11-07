#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

// Network SSID and password
const char* ssid = "ASSEMBLY ONLY";
const char* password = "onlyassemblytebos";

// Host (server) identification - URL
const char* host = "10.16.2.5";
const char* apiPath = "/siap.smknbansari.sch.id/app/directagJSON.php";
const int port = 80;

#define LED_PIN D0  // D8
#define BUZ_PIN D1  // D1

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char IDTAG[20];
char chipID[25];                       // Store ESP8266 Chip ID
char nodevice[20] = "2309G001";      // Change to your desired Node Device (max 20 characters)
char key[50] = "1234567890987654321";  // Change to your desired Key (max 20 characters)

MFRC522 mfrc522(SDA_PIN, RST_PIN);

// Declare jsonResponse as a global variable
String jsonResponse;

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
}

void loop() {
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
      jsonResponse = sendCardIdToServer(IDTAG);
      // Serial.print("1. jsonResponse: ");
      // Serial.println(jsonResponse);

      // Parse and process JSON response
      if (!jsonResponse.isEmpty()) {
        identifyAndProcessJsonResponse(jsonResponse);
      } else {
        Serial.println("jsonResponse: NULL");
      }

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
  jsonResponse = "";
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
  WiFiClient client;
  int maxRetries = 3;  // Maksimum percobaan ulang
  int retryCount = 0;

  while (retryCount < maxRetries) {
    if (!client.connect(host, port)) {
      Serial.println("Failed to connect to the server");
      return "";
    }

    Serial.println("Kirim GET");

    // Send RFID card data, Chip ID, Node Device, and Key to the server
    String requestUrl = String(apiPath) + "?nokartu=" + cardId + "&idchip=" + chipID + "&nodevice=" + nodevice + "&key=" + key;
    client.print(String("GET ") + requestUrl + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    unsigned long startTime = millis();
    while (client.connected() && millis() - startTime < 10000) {
      if (client.available()) {
        char c = client.read();
        jsonResponse += c;
      }
    }
    client.stop();

    if (!jsonResponse.isEmpty()) {
      // Jika jsonResponse tidak kosong, keluar dari loop
      Serial.println("Dapat JSON");
      break;
    }

    // Tunggu sebentar sebelum mencoba kembali
    delay(1000);

    // Tambahkan penghitung percobaan
    Serial.print("jmlGET: ");
    Serial.println(retryCount);
    retryCount++;
  }

  if (jsonResponse.isEmpty()) {
    Serial.println("No response after maximum retries");
  }

  return jsonResponse;
}

void identifyAndProcessJsonResponse(String jsonResponse_0) {
  Serial.print("jsonResponse: ");
  Serial.println(jsonResponse_0);

  // Temukan indeks karakter '{' pertama dan indeks karakter '}' terakhir
  int startIndex = jsonResponse_0.indexOf("{");
  int endIndex = jsonResponse_0.lastIndexOf("}");

  // Ekstrak JSON dari respons HTTP
  String jsonPayload = jsonResponse_0.substring(startIndex, endIndex + 1);

  Serial.print("JSON Payload:");
  Serial.println(jsonPayload);

  StaticJsonDocument<1024> jsonDoc;

  DeserializationError error = deserializeJson(jsonDoc, jsonPayload);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  } else {
    // Access JSON elements directly
    const char* id = jsonDoc["respon"][0]["id"];
    const char* nodevice = jsonDoc["respon"][0]["nodevice"];
    const char* message = jsonDoc["respon"][0]["message"];
    const char* info = jsonDoc["respon"][0]["info"];
    const char* nokartu = jsonDoc["respon"][0]["nokartu"];

    // Print the values
    Serial.println("JSON Elements:");
    Serial.print("id: ");
    Serial.println(id);
    Serial.print("nodevice: ");
    Serial.println(nodevice);
    Serial.print("message: ");
    Serial.println(message);
    Serial.print("info: ");
    Serial.println(info);
    Serial.print("nokartu: ");
    Serial.println(nokartu);
  }

  Serial.println("");
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
