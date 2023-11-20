#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
char barisPertama[100];
char barisKedua[100];

boolean aktifSerialMsg = false;

// Ganti true jika berhotspot tanpa password
boolean modeHotspot = false;
const char* hotspot = "HOTSPOT-SKANEBA";

// HOTSPOT ITC
// const char* ssid = "HOTSPOT-SKANEBA";
// const char* password = "itbisa123";

// WiFi credentials
// ASSEMBLY TE - CNC
const char* ssid = "ASSEMBLY ONLY";
const char* password = "onlyassemblytebos";

// INSTRUKTUR TE - 2.4G
// const char* ssid = "INTRUKTUR-TAV-2.4G";
// const char* password = "skanebabisa1";

//HOSTSPOT-SKANEBA-TU
// const char* ssid = "HOTSPOT-SKANEBA-TU";
// const char* password = "skanebabisa";

// MQTT Broker Configuration
// char nodevice[20] = "2309G004";  // GERBANG / PRESENSI MASUK (max 20 characters)
char nodevice[20] = "2309MAS005";  // PEMBIASAAN MASJID (max 20 characters)
// char nodevice[20] = "2309IZ001";      // POS SATPAM (IJIN) (max 20 characters)
// char nodevice[20] = "2309NA003";  // PEMBIASAAN MASJID (max 20 characters)

const char* mqtt_server = "172.16.80.123";  // Ganti dengan alamat IP broker MQTT Anda
// const char* mqtt_server = "10.16.0.102";  // Ganti dengan alamat IP broker MQTT Anda

const int mqtt_port = 1883;          // Port MQTT default
const char* mqtt_user = "ben";       // Username MQTT Anda
const char* mqtt_password = "1234";  // Password MQTT Anda

#define BUZ_PIN D0  // D0 - MERAH
// #define LED_PIN D1  // D1 - BIRU - BUZZER
// #define OKE_PIN D2  // D2 - HIJAU
#define SET_BTN D8  // Push BUtton SET

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char IDTAG[20];
char chipID[25];                       // Store ESP8266 Chip ID
char key[50] = "1234567890987654321";  // Change to your desired Key (max 20 characters)
char buff[10];

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
int blinkLCD1 = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");

  lcd.setCursor(0, 0);
  lcd.print(">>>>> SiAPP >>>>");
  lcd.setCursor(0, 1);
  lcd.print("Memulai System..");

  delay(1000);

  // pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  // pinMode(OKE_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT_PULLUP);

  if (modeHotspot == true) {
    const char* ssid = hotspot;
  }

  if (modeHotspot == true) {
    WiFi.begin(ssid);
  } else {
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    // blink(3, 100, 400);
    Serial.print(".");

    lcd.setCursor(0, 1);
    lcd.print("M'hubungkan WiFi");
  }

  // digitalWrite(LED_PIN, HIGH);
  Serial.println();
  Serial.println("Tersambung ke WiFi");
  Serial.println();

  lcd.setCursor(0, 1);
  lcd.print("WiFi Tersambung!");

  // Setup MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
      Serial.println("Tersambung ke MQTT Broker");
      lcd.setCursor(0, 1);
      lcd.print("Konek ke Server!");
      buzzBasedOnMessage("200");
    } else {
      Serial.println("Koneksi MQTT gagal. Mengulangi koneksi...");

      lcd.setCursor(0, 1);
      lcd.print("Gagal Konek Svr!");

      // blink(5, 100, 1000);
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

  // Subscribe to a topic
  String topic = "responServer_";
  topic += nodevice;
  client.subscribe(topic.c_str(), 0);

  buzz(3);

  Serial.println("Tempelkan kartu RFID");
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (aktifSerialMsg) {
    Serial.print("Menerima pesan pada topic: ");
    Serial.println(topic);
  }

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

void homeLCD() {
  if (blinkLCD1 == 0) {
    lcd.setCursor(0, 0);
    lcd.print("    SiAPP ");
  } else {
    lcd.setCursor(0, 0);
    lcd.print(">>> SiAPP ");
  }

  int rssi = WiFi.RSSI();
  lcd.setCursor(10, 0);
  if (rssi < (-60)) {
    lcd.print("******");
  } else if (rssi >= (-60)) {
    lcd.print("*****_");
  } else if (rssi >= (-70)) {
    lcd.print("****__");
  } else if (rssi >= (-80)) {
    lcd.print("***___");
  } else if (rssi >= (-90)) {
    lcd.print("**____");
  } else if (rssi >= (-100)) {
    lcd.print("*_____");
  }
}

void loop() {
  homeLCD();

  lcd.setCursor(0, 1);
  lcd.print("Tempelkan Kartu!");

  client.loop();

  // Ambil waktu sekarang
  unsigned long currentMillisLEDmqtt = millis();

  // Indikator Koneksi LED JIka konek Kedil Slow, Jika diskonek kedip cepet
  if (!client.connected()) {
    if (currentMillisLEDmqtt - previousLEDmqtt >= intervalLEDmqtt2) {
      previousLEDmqtt = currentMillisLEDmqtt;

      if (blinkLCD1 == 0) {
        blinkLCD1++;
      } else {
        blinkLCD1 = 0;
      }
    }
  } else {
    if (currentMillisLEDmqtt - previousLEDmqtt >= intervalLEDmqtt) {
      previousLEDmqtt = currentMillisLEDmqtt;
      if (blinkLCD1 == 0) {
        blinkLCD1++;
      } else {
        blinkLCD1 = 0;
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

    lastRFIDReadTime = millis();    // Perbarui waktu terakhir pembacaan kartu RFID
    static char hasilTAG[20] = "";  // Store previous tag ID

    if (strcmp(hasilTAG, IDTAG) != 0) {
      strcpy(hasilTAG, IDTAG);

      if (aktifSerialMsg) {
        Serial.println("ID Kartu: " + String(IDTAG));
      }

      // Send data to server and receive JSON response
      String jsonResponse = sendCardIdToServer(IDTAG);

      // digitalWrite(OKE_PIN, LOW);
      // digitalWrite(LED_PIN, LOW);

      if (aktifSerialMsg) {
        Serial.println("Selesai kirim untuk ID: " + String(IDTAG));
      }

      lcd.setCursor(0, 0);
      lcd.print(">>>>>>>>>>>>>>>>");
      lcd.setCursor(0, 1);
      lcd.print("Kirim Data......");
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

  // Periksa apakah sudah waktunya untuk restart
  unsigned long currentTime = millis();
  if (currentTime - lastRFIDReadTime > RFID_READ_INTERVAL) {
    Serial.println("Tidak ada aktifitas pembacaan kartu RFID selama 10 menit. Melakukan restart...");
    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>>>>>>>>");
    lcd.setCursor(0, 1);
    lcd.print("Restarting... 3s");
    buzzBasedOnMessage("400");
    delay(3000);
    ESP.restart();
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
    // Serial.println("Tersambung ke MQTT Broker");
    // Serial.println("Kirim ke topik: " + mqttTopic + ": " + request);
    client.publish(mqttTopic.c_str(), request.c_str(), 0);

    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>>>>>>>>");
    lcd.setCursor(0, 1);
    lcd.print("Data Terkirim !!");
  } else {
    buzzBasedOnMessage("400");
    Serial.println("Koneksi ke MQTT Broker gagal");

    lcd.setCursor(0, 0);
    lcd.print("xxxxxxxxxxxxxxxx");
    lcd.setCursor(0, 1);
    lcd.print("Koneksi Gagal!!!");
    reconnect();
  }

  return jsonResponse;
}

void reconnect() {
  // Loop sampai terhubung ke broker MQTT
  while (!client.connected()) {
    Serial.println("Menyambungkan ke MQTT Broker...");
    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>>>>>>>>");
    lcd.setCursor(0, 1);
    lcd.print("Menyambung Ulang");
    // Coba terhubung ke broker MQTT
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
      Serial.println("Tersambung ke MQTT Broker");
      lcd.setCursor(0, 0);
      lcd.print(">>>>>>>>>>>>>>>>");
      lcd.setCursor(0, 1);
      lcd.print("Konek Server....");

      // Langganan topik yang Anda butuhkan di sini jika diperlukan
      String topic = "responServer_";
      topic += nodevice;
      client.subscribe(topic.c_str(), 0);
    } else {
      buzzBasedOnMessage("400");

      Serial.print("MQTT Gagal, rc=");
      Serial.print(client.state());
      Serial.println("mencoba konek lagi dalam 5 detik");

      lcd.setCursor(0, 0);
      lcd.print(">>>>>>>>>>>>>>>>");
      lcd.setCursor(0, 1);
      lcd.print("Menyambung Ulang");
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

void buzz_er(String _kode) {
  for (int i = 0; i < _kode.length(); i++) {
    char karakter = _kode.charAt(i);

    if (karakter == '_') {
      // Buzzer berbunyi selama 1 detik
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 1 detik
      delay(1000);
      // Matikan buzzer
      digitalWrite(BUZ_PIN, LOW);
    } else if (karakter == '.') {
      // Buzzer berbunyi selama 100 mili detik
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 100 mili detik
      delay(100);
      // Matikan buzzer
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
      // Serial.println(buzzerCodes[i + 1]);
      break;  // Keluar dari loop setelah menemukan kode yang cocok
    }
  }
}

void identifyAndProcessJsonResponse(String jsonResponse, char* _nodevice) {
  const char* pesanJSON = "";
  const char* pesanLCD = "";

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
    pesanJSON = "500";
    Serial.print("gagal to parse JSON: ");
    Serial.println(error.c_str());

    lcd.setCursor(0, 0);
    lcd.print("xxxxxxxxxxxxxxxx");
    lcd.setCursor(0, 1);
    lcd.print("Gagal Olah Pesan");
  } else {
    // Mengakses elemen-elemen JSON yang benar
    const char* json_id = jsonDoc["respon"][0]["id"];
    const char* json_nodevice = jsonDoc["respon"][0]["nodevice"];
    const char* json_message = jsonDoc["respon"][0]["message"];
    const char* json_info = jsonDoc["respon"][0]["info"];
    const char* json_nokartu = jsonDoc["respon"][0]["nokartu"];

    // // Temukan posisi koma
    // const char* posKoma = strchr(json_info, '--');

    // // Ambil data setelah koma
    // int indexKoma = posKoma - json_info;
    // const char* dataSetelahKoma = json_info + indexKoma + 2;

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
        if (aktifSerialMsg) {
          Serial.print("ID & Nomor Device Sesuai! ");
          Serial.println();
        }

        // explodeAndCombine(dataSetelahKoma, barisPertama, barisKedua);
        splitAndRemoveDash(json_info, barisPertama, barisKedua);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(barisPertama);
        lcd.setCursor(0, 1);
        lcd.print(barisKedua);
        pesanJSON = json_message;
        json_info = "";
      } else {
        Serial.println("ID & Nomor Device Tidak Sesuai...!");
        Serial.println("Permintaan tidak direspon.");

        lcd.setCursor(0, 0);
        lcd.print("xxxxxxxxxxxxxxxx");
        lcd.setCursor(0, 1);
        lcd.print("Tertolak Server!");
        pesanJSON = "501";
      }

      // posKoma = "";
      // indexKoma = 0;
      // dataSetelahKoma = "";
    } else {
      // Elemen "nodevice" tidak ada dalam JSON
      Serial.println("Elemen \"nodevice\" tidak ada dalam JSON.");
      Serial.println("Permintaan tidak direspon.");

      lcd.setCursor(0, 0);
      lcd.print("xxxxxxxxxxxxxxxx");
      lcd.setCursor(0, 1);
      lcd.print("Tdk ada respon!!");

      pesanJSON = "502";
    }
  }

  // Setelah selesai memproses respon, putuskan koneksi MQTT
  client.disconnect();
  // Serial.println("Koneksi Selesai");
  // aktikan Buzz sesuai KOde Pesan
  buzzBasedOnMessage(pesanJSON);
  delay(1000);
}

void explodeAndCombine(const char* kalimat, char* barisPertama, char* barisKedua) {
  // Inisialisasi variabel untuk menyimpan hasil
  strcpy(barisPertama, "");
  strcpy(barisKedua, "");

  // Pisahkan kata-kata
  char* kata = strtok(const_cast<char*>(kalimat), " ");
  int panjangBarisPertama = 0;

  // Gabungkan kata-kata hingga mencapai atau melebihi 16 karakter pada barisPertama
  while (kata != NULL) {
    if (panjangBarisPertama + strlen(kata) + 1 <= 16) {
      // Gabungkan kata ke barisPertama
      strcat(barisPertama, kata);
      strcat(barisPertama, " ");  // Tambahkan spasi di antara kata
      panjangBarisPertama += strlen(kata) + 1;
    } else {
      // Pindah ke barisKedua jika panjang batas tercapai
      strcpy(barisKedua, kata);
      while ((kata = strtok(NULL, " ")) != NULL) {
        strcat(barisKedua, " ");
        strcat(barisKedua, kata);
      }
      break;
    }
    kata = strtok(NULL, " ");
  }

  // Hilangkan spasi ekstra di akhir barisPertama
  if (barisPertama[strlen(barisPertama) - 1] == ' ') {
    barisPertama[strlen(barisPertama) - 1] = '\0';
  }
}

void splitAndRemoveDash(const char* json_info, char* barisPertama, char* barisKedua) {
  // Inisialisasi variabel untuk menyimpan hasil
  strcpy(barisPertama, "");
  strcpy(barisKedua, "");

  // Temukan substring "--"
  const char* posDoubleDash = strstr(json_info, "--");

  // Jika ditemukan, pisahkan string
  if (posDoubleDash != NULL) {
    // Hitung panjang string sebelum --
    size_t panjangBarisPertama = posDoubleDash - json_info;

    // Salin bagian sebelum --
    strncat(barisPertama, json_info, panjangBarisPertama);

    // Salin bagian setelah --
    strcpy(barisKedua, posDoubleDash + 2);
  } else {
    // Jika tidak ditemukan, set barisKedua sebagai string kosong
    strcpy(barisKedua, json_info);
  }
}