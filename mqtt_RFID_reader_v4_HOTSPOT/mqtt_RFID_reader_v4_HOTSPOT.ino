#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

boolean aktifSerialMsg = false;
boolean autoRestart = false;

// Ganti true jika berhotspot tanpa password
boolean modeHotspot = false;
// const char* ssid = "HOTSPOT-SKANEBA";
// const char* password = "";

// HOTSPOT ITC
// const char* ssid = "HOTSPOT-SKANEBA";
// const char* password = "itbisa123";

// WiFi credentials
// ASSEMBLY TE - CNC
// const char* ssid = "ASSEMBLY ONLY";
// const char* password = "onlyassemblytebos";

// INSTRUKTUR TE - 2.4G
const char* ssid = "INTRUKTUR-TAV-2.4G";
const char* password = "skanebabisa1";

// FABRIKASI TE / LGT
// const char* ssid = "MuT@n";
// const char* password = "n0mut4nn";

// INSTRUKTUR MM - 2.4G
// const char* ssid = "INTRUKTUR-MM-2.4";
// const char* password = "skanebabisa";

//HOSTSPOT-SKANEBA-TU
// const char* ssid = "HOTSPOT-SKANEBA-TU";
// const char* password = "skanebabisa";

// MQTT Broker Configuration
// char nodevice[20] = "2309G005";  // GERBANG / PRESENSI MASUK (max 20 characters)
// char nodevice[20] = "2309MAS004";  // PEMBIASAAN MASJID (max 20 characters)
char nodevice[20] = "2309IZ001";      // POS SATPAM (IJIN) (max 20 characters)
// char nodevice[20] = "2309NA003";  // PEMBIASAAN MASJID (max 20 characters)

const char* mqtt_server = "172.16.80.123";  // Ganti dengan alamat IP broker MQTT Anda
// const char* mqtt_server = "10.16.0.102";  // Ganti dengan alamat IP broker MQTT Anda

const int mqtt_port = 1883;          // Port MQTT default
const char* mqtt_user = "ben";       // Username MQTT Anda
const char* mqtt_password = "1234";  // Password MQTT Anda

#define LED_PIN D0  // D0 - MERAH
#define BUZ_PIN D1  // D1 - BIRU - BUZZER
#define OKE_PIN D2  // D2 - HIJAU
#define SET_BTN D8  // Push BUtton SET

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char IDTAG[20];
char chipID[25];                       // Store ESP8266 Chip ID
char key[50] = "1234567890987654321";  // Change to your desired Key (max 20 characters)

// Membuat array untuk memetakan pesan ke kode bunyi buzzer
const char* buzzerCodes[] = {
  // error
  "400", "_ _",
  "404", "_..._",
  "405", "_...._",
  "406", "_....._",
  "407", "_....._",
  "500", "_....._",
  "501", "_._..._",
  "502", "_._....._",
  "505", "_....._._",
  "515", "_....._",
  "545", "_....._",
  "555", "_....._",
  // akses
  "IDTT", "._..",
  "HLTM", "_.",
  "TBPS", "._._..",
  "TASK", "_._..",
  "PLAW", "_..",
  // OK
  "200", "..",
  "SAPP", "...",
  "PPBH", "..",
  "510", "...",
  "PPPP", "...",
  "SMPM", "...",
  "MMMM", "...",
  "BMPM", "..",
  "PKBD", "..",
  "BMPE", "..",
  "BPSE", "..",
  "BPEB", "..",
  "BMIJ", ".."
};

String receivedMessage = "";

MFRC522 mfrc522(SDA_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

// Variabel untuk memantau waktu terakhir pembacaan kartu RFID
unsigned long lastRFIDReadTime = 0;
const unsigned long RFID_READ_INTERVAL = 600000;  // 10 menit dalam milidetik (10 * 60 * 1000)

unsigned long previousLEDmqtt = 0;
const long intervalLEDmqtt = 100;
const long intervalLEDmqtt2 = 500;

int tick = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  pinMode(OKE_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT_PULLUP);

  if (modeHotspot == true) {
    WiFi.begin(ssid);
  } else {
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    blink(3, 100, 400);
    Serial.print(".");
  }

  digitalWrite(LED_PIN, HIGH);
  Serial.println();
  Serial.println("Tersambung ke WiFi");
  Serial.println();

  // Setup MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
      Serial.println("Tersambung ke MQTT Broker");
      buzzBasedOnMessage("200");
    } else {
      Serial.println("Koneksi MQTT gagal. Mengulangi koneksi...");

      blink(5, 100, 1000);
    }
  }

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
  Serial.println("Tempelkan kartu RFID");

  // Subscribe to a topic
  String topic = "responServer_";
  topic += nodevice;
  client.subscribe(topic.c_str(), 0);

  buzz(3);
  client.disconnect();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Menerima pesan pada topic: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    receivedMessage += (char)payload[i];
  }

  if (aktifSerialMsg) {
    Serial.print("Pesan: ");
    Serial.println(receivedMessage);
    Serial.println();
  }

  // prosen respon data
  identifyAndProcessJsonResponse(receivedMessage, nodevice);
}

void loop() {
  client.loop();

  // Ambil waktu sekarang
  unsigned long currentMillisLEDmqtt = millis();

  // Indikator Koneksi LED JIka konek Kedil Slow, Jika diskonek kedip cepet
  if (!client.connected()) {
    if (tick < 1) {
      tick = 1;
      Serial.println("[Stanby] Tidak terkoneksi ke Server");
      Serial.println();
    }
    tick++;
    digitalWrite(OKE_PIN, LOW);

    if (currentMillisLEDmqtt - previousLEDmqtt >= intervalLEDmqtt2) {
      previousLEDmqtt = currentMillisLEDmqtt;

      if (digitalRead(LED_PIN) == LOW) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }
    }
  } else {
    if (tick < 1) {
      tick = 1;
      Serial.println("Menyambungkan ke Server.. ");
      Serial.println();
    }
    tick++;

    if (currentMillisLEDmqtt - previousLEDmqtt >= intervalLEDmqtt) {
      previousLEDmqtt = currentMillisLEDmqtt;

      if (digitalRead(LED_PIN) == LOW) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }
    }
  }

  int berhasilBaca = bacaTag();


  if (berhasilBaca) {
    tick = 0;
    if (IDTAG) {
      buzz(1);
      reconnect();
    }

    if (autoRestart)
      lastRFIDReadTime = millis();  // Perbarui waktu terakhir pembacaan kartu RFID
      
    static char hasilTAG[20] = "";  // Store previous tag ID

    if (strcmp(hasilTAG, IDTAG) != 0) {
      strcpy(hasilTAG, IDTAG);

      if (aktifSerialMsg) {
        Serial.println();
        Serial.println("=======================");
        Serial.println("ID Kartu: " + String(IDTAG));
      }

      // Send data to server and receive JSON response
      String jsonResponse = sendCardIdToServer(IDTAG);

      digitalWrite(OKE_PIN, LOW);
      digitalWrite(LED_PIN, LOW);

      if (aktifSerialMsg) {
        Serial.println("Selesai kirim untuk ID: " + String(IDTAG));
        Serial.println("=======================");
        Serial.println("");
      }

    } else {
      Serial.println("-");
    }

    delay(900);
    strcpy(hasilTAG, "");
  } else {
    strcpy(IDTAG, "");
  }

  buzz(0);
  receivedMessage = "";

  if (autoRestart) {
    // Periksa apakah sudah waktunya untuk restart
    unsigned long currentTime = millis();
    if (currentTime - lastRFIDReadTime > RFID_READ_INTERVAL) {
      Serial.println("Tidak ada aktifitas pembacaan kartu RFID selama 10 menit. Melakukan restart...");
      ESP.restart();
    }
  }
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
  request += "\"key\":\"" + String(key) + "\",";
  request += "\"ipa\":\"" + WiFi.localIP().toString() + "\"";
  request += "}";

  if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
    String mqttTopic = "dariMCU_" + String(nodevice);
    Serial.println("Tersambung ke MQTT Broker");
    Serial.println("Kirim ke topik: " + mqttTopic + ": " + request);
    client.publish(mqttTopic.c_str(), request.c_str(), 0);

    ledSuccess(true);
  } else {
    ledSuccess(false);

    buzzBasedOnMessage("400");
    Serial.println("Koneksi ke MQTT Broker gagal");

    reconnect();
  }

  return jsonResponse;
}

void reconnect() {
  // Loop sampai terhubung ke broker MQTT
  while (!client.connected()) {
    ledSuccess(false);

    Serial.println("Menyambungkan ke MQTT Broker...");
    // Coba terhubung ke broker MQTT
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {

      Serial.println("Tersambung ke MQTT Broker");

      // Langganan topik yang Anda butuhkan di sini jika diperlukan
      String topic = "responServer_";
      topic += nodevice;
      client.subscribe(topic.c_str(), 0);

      ledSuccess(true);
    } else {
      ledSuccess(false);

      buzzBasedOnMessage("400");

      Serial.print("MQTT Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba konek lagi dalam 5 detik");
      // Tunggu sebelum mencoba lagi
      blink(5, 100, 2000);
    }
  }
}

void buzz(int loop) {
  if (loop == 0) {
    digitalWrite(BUZ_PIN, LOW);
  } else if (loop == 1) {
    digitalWrite(BUZ_PIN, HIGH);
    delay(100);
    digitalWrite(BUZ_PIN, LOW);
  } else {
    for (int t = 0; t < loop; t++) {
      digitalWrite(BUZ_PIN, HIGH);
      delay(100);
      digitalWrite(BUZ_PIN, LOW);
      delay(100);
    }
  }
}

void blink(int _loop, int ms_, int _ms) {
  for (int r = 0; r < _loop; r++) {
    digitalWrite(LED_PIN, HIGH);
    delay(ms_);
    digitalWrite(LED_PIN, LOW);
    delay(ms_);
  }
  delay(_ms);
}

void ledSuccess(bool state) {
  if (state) {
    digitalWrite(OKE_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(OKE_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
  }
}

void buzz_er(String _kode) {
  for (int i = 0; i < _kode.length(); i++) {
    char karakter = _kode.charAt(i);

    if (karakter == '_') {
      // Buzzer berbunyi selama 1 detik
      // Frekuensi bunyi buzzer (1 kHz)
      // tone(BUZ_PIN, 1000);
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 1 detik
      delay(1000);
      // Matikan buzzer
      // noTone(BUZ_PIN);
      digitalWrite(BUZ_PIN, LOW);
    } else if (karakter == '.') {
      // Buzzer berbunyi selama 100 mili detik
      // Frekuensi bunyi buzzer (1 kHz)
      // tone(BUZ_PIN, 1000);
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 100 mili detik
      delay(100);
      // Matikan buzzer
      // noTone(BUZ_PIN);
      digitalWrite(BUZ_PIN, LOW);
    } else if (karakter == ' ') {
      // Tunda 100 mili detik
      delay(100);
    }

    // Tunda sebelum karakter selanjutnya (jika ada)
    if (i < _kode.length() - 1) {
      delay(100);  // Tunda 100 mili detik sebelum karakter berikutnya
    }
  }

  delay(1000);
}

void buzzBasedOnMessage(const char* message) {
  for (int i = 0; i < sizeof(buzzerCodes) / sizeof(buzzerCodes[0]); i += 2) {
    if (strcmp(message, buzzerCodes[i]) == 0) {
      buzz_er(buzzerCodes[i + 1]);
      Serial.println(buzzerCodes[i + 1]);
      break;  // Keluar dari loop setelah menemukan kode yang cocok
    }
  }
}

void identifyAndProcessJsonResponse(String jsonResponse, char* _nodevice) {
  const char* pesanJSON = "";

  // Parse and process JSON response
  // Menghapus karakter newline dan carriage return
  jsonResponse.replace("\n", "");
  jsonResponse.replace("\r", "");
  // Menghapus karakter backslash yang mengganggu
  jsonResponse.replace("\\", "");

  // Menghapus karakter ganda ("") dari awal dan akhir JSON
  jsonResponse = jsonResponse.substring(1, jsonResponse.length() - 1);
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, jsonResponse);

  if (error) {
    ledSuccess(false);

    pesanJSON = "500";
    Serial.print("gagal to parse JSON: ");
    Serial.println(error.c_str());
  } else {
    // Mengakses elemen-elemen JSON yang benar
    const char* json_id = jsonDoc["respon"][0]["id"];
    const char* json_nodevice = jsonDoc["respon"][0]["nodevice"];
    const char* json_message = jsonDoc["respon"][0]["message"];
    const char* json_info = jsonDoc["respon"][0]["info"];
    const char* json_nokartu = jsonDoc["respon"][0]["nokartu"];

    if (json_nodevice) {
      if (aktifSerialMsg) {
        Serial.print("- id: ");
        Serial.println(json_id);
        Serial.print("- nodevice asal: ");
        Serial.println(_nodevice);
        Serial.print("- nodevice json: ");
        Serial.println(json_nodevice);
        Serial.print("- pesan: ");
        Serial.println(json_message);
        Serial.print("- info: ");
        Serial.println(json_info);
        Serial.print("- nokartu: ");
        Serial.println(json_nokartu);
      }

      if (strcmp(_nodevice, json_nodevice) == 0 && strcmp("406", json_message) != 0) {
        ledSuccess(true);

        if (aktifSerialMsg) {
          Serial.print("ID & Nomor Device Sesuai! ");
          Serial.println();
        }

        pesanJSON = json_message;
      } else {
        ledSuccess(false);

        Serial.println("ID & Nomor Device Tidak Sesuai...!");
        Serial.println("Permintaan tidak direspon.");

        pesanJSON = "501";
      }
    } else {
      ledSuccess(false);

      // Elemen "nodevice" tidak ada dalam JSON
      Serial.println("Elemen \"nodevice\" tidak ada dalam JSON.");
      Serial.println("Permintaan tidak direspon.");

      pesanJSON = "502";
    }
  }

  // Setelah selesai memproses respon, putuskan koneksi MQTT
  client.disconnect();
  Serial.println("Koneksi Selesai");

  // aktikan Buzz sesuai KOde Pesan
  buzzBasedOnMessage(pesanJSON);
}