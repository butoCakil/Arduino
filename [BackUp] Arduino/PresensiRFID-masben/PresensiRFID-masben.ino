#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>

//Network SSID
const char* ssid = "ASSEMBLY ONLY";
const char* password = "onlyassemblytebos";

//pengenal host (server) = IP Address komputer server
// const char* host = "siap.smknbansari.sch.id";
const char* host = "10.16.2.5";
const int port = 80;

#define LED_PIN D0  //D8
#define BUZ_PIN 5   //D1

//sediakan variabel untuk RFID
#define SDA_PIN 2  //D4
#define RST_PIN 0  //D3

char IDTAG[20];
static char hasilTAG[20] = "";
int i, j, berhasilBaca;
unsigned long timeout;
String dataMasuk, hasilData;
boolean parsing = false;
int loadInfo;
unsigned long previousMillis, currentMillis;
char chipID[25];

int ledState = LOW;

MFRC522 mfrc522(SDA_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  pinMode(BUZ_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  buzz(0);

  //setting koneksi wifi
  WiFi.hostname("NodeMCU");
  WiFi.begin(ssid, password);

  {
    //cek koneksi wifi
    Serial.println("Menyambungkan ke Wifi");
    Serial.println();
  }

  while (WiFi.status() != WL_CONNECTED) {
    //progress sedang mencari WiFi
    Serial.print(".");
    delay(500);
  }

  {
    Serial.println();
    Serial.println("Tersambung ke Wifi");
    Serial.println("IP Address : ");
    Serial.println(WiFi.localIP());
    Serial.println("MAC Address : ");
    Serial.println(WiFi.macAddress());
  }

  {
    int num = ESP.getChipId();
    itoa(num, chipID, 10);
    Serial.println("ID chip: ");
    Serial.println(chipID);
  }

  {
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("Tempelkan Kartu RFID Anda ke Reader");
    Serial.println();
  }

  {
    buzz(3);
  }
}

void loop() {
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.print(".");
    i = 0;
    return;
  }
  if (i == 0) {
    Serial.println("");
    Serial.println("Koneksi OK");
    Serial.println("");
    i = 1;
  }

  berhasilBaca = bacaTag();

  if (berhasilBaca) {
    if (IDTAG) {
      buzz(1);
    } else {
      buzz(0);
    }

    if (strcmp(hasilTAG, IDTAG) != 0) {
      strcpy(hasilTAG, IDTAG);
      Serial.println("Nomor ID Kartu :" + String(IDTAG));
      //          String apiUrl = "/presensi/kirimkartu.php?nokartu=" + IDTAG;
      String apiUrl = "/siap.smknbansari.sch.id/tag.php?nokartu=" + String(IDTAG);

      client.print(String("GET ") + apiUrl + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

      // while (client.available()) {
      //   String linex = client.readStringUntil('\r');
      //   if (linex) { Serial.println("Kartu terkirim"); }
        // String hasil = client.find();
        // Serial.println(hasil);
        // String baca = client.read();
        // Serial.println(baca);
      // }
    } else {
      Serial.println("...");
    }

    delay(1000);
  } else {
    strcpy(hasilTAG, "");
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