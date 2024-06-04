#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SS_PIN 2
#define RST_PIN 0

// #define SDA_PIN 2  // D4
// #define RST_PIN 0  // D3

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Deklarasi variabel untuk menyimpan data
String id_siswa;
String nama_siswa;
String kelas_siswa;

// Pengaturan WiFi
const char* ssid = "INTRUKTUR-TAV-2.4G";
const char* password = "skanebabisa1";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Inisialisasi variabel
  id_siswa = "";
  nama_siswa = "";
  kelas_siswa = "";

  // Menghubungkan ke WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung!");
}

void loop() {
  // Membaca data dari RFID tag
  if (mfrc522.PICC_IsNewCardPresent()) {
    mfrc522.PICC_ReadCardSerial();

    // Mengubah data UID menjadi string
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      id_siswa += String(mfrc522.uid.uidByte[i], HEX);
    }

    // Mendapatkan data siswa dari database (contoh)
    // Ganti kode ini dengan cara Anda mendapatkan data siswa berdasarkan ID
    if (id_siswa == "13197d5") {
      nama_siswa = "Budi";
      kelas_siswa = "X IPA 1";
    } else if (id_siswa == "94ed9551") {
      nama_siswa = "Ani";
      kelas_siswa = "XI IPS 2";
    } else {
      nama_siswa = "Data tidak ditemukan";
      kelas_siswa = "";
    }

    // Mencetak data ke serial monitor
    Serial.println("ID Siswa: " + id_siswa);
    Serial.println("Nama Siswa: " + nama_siswa);
    Serial.println("Kelas Siswa: " + kelas_siswa);
    Serial.println();

    // Mengirim data ke Google Spreadsheet
    kirimDataKeGoogleSpreadsheet(id_siswa, nama_siswa, kelas_siswa);

    // Mengosongkan variabel
    id_siswa = "";
    nama_siswa = "";
    kelas_siswa = "";
  }
}

// ... other code

void kirimDataKeGoogleSpreadsheet(String id, String nama, String kelas) {
  // Buatlah URL Google Spreadsheet Anda di sini
  String url = "https://script.google.com/macros/s/AKfycbyasRn625sO8oHLupz3_SKGDYwL38WGf2VlEgsePXfQ0HhSd5boOVTUPKXKOUMBrlUQ/exec";

  // Buatlah body request sesuai dengan format Google Apps Script
  String body = "id=" + id + "&nama=" + nama + "&kelas=" + kelas;

  // Buatlah HTTP request dengan cara yang benar
  WiFiClient client;  // Create a WiFiClient object
  HTTPClient http;
  http.begin(client, url);  // Use the updated begin() method

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(body);

  // Periksa status HTTP
  if (httpCode == 200) {
    Serial.println("Data berhasil dikirim ke Google Spreadsheet");
  } else {
    Serial.println("Gagal mengirim data ke Google Spreadsheet (kode: " + String(httpCode) + ")");
  }

  http.end();
}
