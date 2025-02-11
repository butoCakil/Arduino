#include "core_esp8266_features.h"
// Membuat array untuk memetakan pesan ke kode bunyi buzzer
const char* buzzerCodes[] = {
  // error
  "400", "_ _",        // 0
  "404", "_..._",      // 1
  "405", "_...._",     // 2
  "406", "_....._",    // 3
  "407", "_....._",    // 4
  "500", "_....._",    // 5
  "501", "_._..._",    // 6
  "502", "_._....._",  // 7
  "505", "_....._._",  // 8
  "515", "_....._",    // 9
  "545", "_....._",    // 10
  "555", "_....._",    // 11
  // akses
  "IDTT", "._..",    // 12
  "HLTM", "_.",      // 13
  "TBPS", "._._..",  // 14
  "TASK", "_._..",   // 15
  "PLAW", "_..",     // 16
  // OK
  "200", "..",    // 17
  "SAPP", "...",  // 18
  "PPBH", "..",   // 19
  "510", "...",   // 20
  "PPPP", "...",  // 21
  "SMPM", "...",  // 22
  "MMMM", "...",  // 23
  "BMPM", "..",   // 24
  "PKBD", "..",   // 25
  "BMPE", "..",   // 26
  "BPSE", "..",   // 27
  "BPEB", "..",   // 28
  "BMIJ", ".."    // 29
};

void ledSuccess(bool state) {
  if (state == 1) {
    digitalWrite(OKE_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
  } else if (state == 2) {
    digitalWrite(OKE_PIN, HIGH);

  } else {
    digitalWrite(OKE_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
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
  nom = 0;
  for (int i = 0; i < sizeof(buzzerCodes) / sizeof(buzzerCodes[0]); i += 2) {
    if (strcmp(message, buzzerCodes[i]) == 0) {
      buzz_er(buzzerCodes[i + 1]);
      if (aktifSerialMsg)
        Serial.println(buzzerCodes[i + 1]);
      break;  // Keluar dari loop setelah menemukan kode yang cocok
    }
    nom++;
  }
}

void reconnect() {
  // Simpan waktu awal
  unsigned long startTimeBootLoad = millis();

  // Loop sampai terhubung ke broker MQTT atau waktu habis
  while (!client.connected()) {
    ledSuccess(0);

    if (aktifSerialMsg)
      Serial.println("Menyambungkan ke MQTT Broker...");

    // Coba terhubung ke broker MQTT
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
      if (aktifSerialMsg)
        Serial.println("Tersambung ke MQTT Broker");

      // Langganan topik yang Anda butuhkan di sini jika diperlukan
      String topic = "responServer_";
      topic += nodevice;
      client.subscribe(topic.c_str(), 0);

      ledSuccess(1);
      break;  // Keluar dari loop jika terhubung
    } else {
      ledSuccess(0);
      buzzBasedOnMessage("400");

      if (aktifSerialMsg) {
        Serial.print("MQTT Gagal, rc=");
        Serial.print(client.state());
        Serial.println(" mencoba konek lagi");
      }

      delay(5000);  // Tunggu 5 detik sebelum mencoba lagi

      // Periksa apakah waktu lebih dari 10 detik
      if (millis() - startTimeBootLoad > 10000) {
        Serial.println("Batas waktu reconnect habis. Keluar dari loop reconnect.");
        break;  // Keluar dari loop jika waktu habis
      }
    }
  }
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

    if (aktifSerialMsg) {
      Serial.println("Tersambung ke MQTT Broker");
      Serial.println("Kirim ke topik: " + mqttTopic + ": " + request);
    }

    client.publish(mqttTopic.c_str(), request.c_str(), 0);

    ledSuccess(1);
  } else {
    ledSuccess(0);

    buzzBasedOnMessage("400");
    Serial.println("Koneksi ke MQTT Broker gagal");

    reconnect();
  }

  return jsonResponse;
}

void printWifiList() {
  Serial.println("WiFi networks ditemukan:");
  injekHtml = "";
  injekHtml = "<div id=\"listssid\" class=\"listssid\">";

  int ii;
  for (const auto& wifi : wifiList) {
    Serial.print(ii);
    Serial.print(". ");
    Serial.print("SSID: ");
    Serial.print(wifi.ssid);
    Serial.print(", RSSI: ");
    Serial.print(wifi.rssi);
    Serial.print(" dBm, Security: ");
    Serial.println(wifi.security);

    injekHtml += "<a id=\"tmblssid1\" onclick=\"changeSSID('" + wifi.ssid + "')\">" + wifi.ssid + " (" + wifi.rssi + " dBm) " + wifi.security + "</a><br>";

    ii++;
  }

  injekHtml += "</div>";
}

void findWifi() {
  Serial.println("Mencari WiFi...");

  wifiList.clear();

  // Tunggu hingga pemindaian WiFi selesai
  networks = WiFi.scanNetworks();

  // Tambahkan informasi WiFi ke dalam vektor
  for (int i = 0; i < networks; ++i) {

    digitalWrite(BUZ_PIN, HIGH);
    delay(50);
    digitalWrite(BUZ_PIN, LOW);
    delay(50);

    WifiInfo wifi;
    wifi.ssid = WiFi.SSID(i);
    wifi.rssi = WiFi.RSSI(i);

    switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_NONE:
        wifi.security = "Open";
        break;
      case ENC_TYPE_WEP:
        wifi.security = "WEP";
        break;
      case ENC_TYPE_TKIP:
        wifi.security = "WPA/TKIP";
        break;
      case ENC_TYPE_CCMP:
        wifi.security = "WPA2/CCMP";
        break;
      case ENC_TYPE_AUTO:
        wifi.security = "WPA/WPA2";
        break;
      default:
        wifi.security = "Unknown";
        break;
    }
    wifiList.push_back(wifi);
  }

  // Tampilkan hasil pemindaian
  if (networks == 0) {
    Serial.println("WiFi tidak ditemukan. REBOOT / Config Manual");
    injekHtml = "WiFi tidak ditemukan. REBOOT / Config Manual";
    delay(2000);
  } else {
    startTimeBootLoad = millis();
    printWifiList();
  }
}

void searchingWifi() {
  startTimeBootLoad = millis();
  findWifi();
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
    ledSuccess(0);

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
        ledSuccess(1);

        if (aktifSerialMsg) {
          Serial.print("ID & Nomor Device Sesuai! ");
          Serial.println();
        }

        // // Find the position of the search string
        // const char* startPos = strstr(json_info, "--");

        // // If the search string is found, replace it
        // while (startPos != nullptr) {
        //   char buffer[100];  // Adjust the size according to your needs
        //   strncpy(buffer, json_info, startPos - json_info);
        //   buffer[startPos - json_info] = '\0';  // Null-terminate the substring
        //   strcat(buffer, ", ");
        //   strcat(buffer, startPos + strlen("--"));

        //   // Copy the modified string back to the json_info
        //   strcpy(const_cast<char*>(json_info), buffer);

        //   // Find the position of the search string in the modified text
        //   startPos = strstr(json_info, "--");
        // }

        pesanJSON = json_message;
        json_info = "";
      } else {
        ledSuccess(0);

        Serial.println("ID & Nomor Device Tidak Sesuai...!");
        Serial.println("Permintaan tidak direspon.");

        pesanJSON = "501";
      }
    } else {
      ledSuccess(0);

      // Elemen "nodevice" tidak ada dalam JSON
      Serial.println("Elemen \"nodevice\" tidak ada dalam JSON.");
      Serial.println("Permintaan tidak direspon.");

      delay(2000);

      pesanJSON = "502";
    }

    tungguRespon = false;
  }

  // Setelah selesai memproses respon, putuskan koneksi MQTT
  client.disconnect();
  if (aktifSerialMsg)
    Serial.println("Koneksi Selesai");

  // aktikan Buzz sesuai KOde Pesan
  if (pesanJSON) {
    buzzBasedOnMessage(pesanJSON);
    delay(1000);
  }
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