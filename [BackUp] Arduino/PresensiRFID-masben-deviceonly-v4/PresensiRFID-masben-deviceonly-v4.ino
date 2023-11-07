#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>

//Network SSID
const char* ssid = "ESEpro";
const char* password = "sijitekanwolu";

//pengenal host (server) = IP Address komputer server
const char* host = "absensi.smknbansari.sch.id";
const int port = 80;

//#define LED_PIN 2  //D8
#define BUZ_PIN 5  //D1

//sediakan variabel untuk RFID
#define SDA_PIN 2  //D4
#define RST_PIN 0  //D3

String IDTAG;
String hasilTAG;
int i, j, berhasilBaca;
unsigned long timeout;
String dataMasuk, hasilData;
boolean parsing = false;
int loadInfo = 0;

MFRC522 mfrc522(SDA_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  //setting koneksi wifi
  WiFi.hostname("NodeMCU");
  WiFi.begin(ssid, password);

  //cek koneksi wifi
  Serial.println("Menyambungkan ke Wifi");
  Serial.println();

  while (WiFi.status() != WL_CONNECTED)
  {
    //progress sedang mencari WiFi
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
  }

  Serial.println();
  Serial.println("Tersambung ke Wifi");
  Serial.println("IP Address : ");
  Serial.println(WiFi.localIP());

  //pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Siap...");
  Serial.println();
}

void loop() {
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Menyambungkan lagi...");
    i = 0;
    return;
  }
  if (i == 0) {
    Serial.println("Koneksi OK");
    i = 1;
  }

  berhasilBaca = bacaTag();

  if (berhasilBaca) {

    if (hasilTAG != IDTAG) {
      hasilTAG = IDTAG;
      Serial.println("ID baru, terbaca");
    }

    if (IDTAG) {
      digitalWrite(BUZ_PIN, HIGH);
    }

    Serial.println("Nomor ID Kartu :" + IDTAG);
    String apiUrl = "/kirimkartu/add/" + IDTAG;

    client.print(String("GET ") + apiUrl + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    while (client.available()) {
      Serial.println("Sedang mengirim nomor ID");
    }

    delay(500);
  } else {
    digitalWrite(BUZ_PIN, LOW);
    IDTAG = "";
  }
}


int bacaTag() {
  if (! mfrc522.PICC_IsNewCardPresent())
    return 0;

  if (! mfrc522.PICC_ReadCardSerial())
    return 0;

  IDTAG = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    IDTAG += mfrc522.uid.uidByte[i];
  }

  return 1;
}

void parsingData() {
  hasilData = "";
  for (i = 1; i < dataMasuk.length(); i++) {
    //pengecekan tiap karakter dengan karakter (#)
    if (dataMasuk[i] == '#')
    {
      //increment variabel j, digunakan untuk merubah index array penampung
      j++;
    }
    else
    {
      //proses tampung data saat pengecekan karakter selesai.
      if (j == 1) {
        hasilData = hasilData + dataMasuk[i];
        if (!hasilData) {
          Serial.println("GAGAL PARSING");
        }
      }
      else if (j >= 2) {
        j = 0;
        parsing = false;
        dataMasuk = "";
        Serial.println("SELESAI PARSING");
        break;
      }
    }
  }
}
