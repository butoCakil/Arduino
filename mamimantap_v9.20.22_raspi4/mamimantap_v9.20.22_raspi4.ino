// Setting Device - User //
String nomesin = "2209251"; // wajib berbeda setiap device
String kode = "AIRGALON";
String aktifasi = "e807f1fcf82d132f9bb018ca6738a19f";

// Setting Koneksi //
const char *ssid = "INTRUKTUR-TAV-2.4G";
const char *pass = "skanebabisa";

// Setting url //
const char *host = "10.31.0.40";
String url = "/mamimantap/apps/tag.php";
int port = 80;

// Setting time delay pump
int delay_pump = 8000;

// End Setting

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
#include <SPI.h>
#include <MFRC522.h>

#define BUZ D8
#define OUT D0
#define SS_PIN 2
#define RST_PIN 0
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
String strID, val;
String data;
String request_string;
String hasilData;
String dataMasuk;
String aktuator;
int i, j, k = 0;
char chipID[25];

HTTPClient http;

void setup()
{
  Serial.begin(115200);

  pinMode(BUZ, OUTPUT);
  digitalWrite(BUZ, LOW);
  pinMode(OUT, OUTPUT);
  digitalWrite(OUT, HIGH);
  pinMode(D2, INPUT_PULLUP);

  WiFi.disconnect();
  WiFi.begin(ssid, pass);

  delay(100);
  Serial.println("");
  Serial.println("Menyambungkan Wifi..");

  while (((WiFi.status() != WL_CONNECTED)))
  {
    Serial.print(".");
    delay(300);
  }

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("");

  int num = ESP.getChipId();
  itoa(num, chipID, 10);
  Serial.println("ID chip: ");
  Serial.println(chipID);

  Serial.println("Koneksi Berhasil!");
  Serial.println("");
  Serial.println("Tempelkan kartu ID ...");

  beep1x();
}

void loop()
{

  while (client.available() > 0)
  {
    delay(1);
    char c = client.read();
    data += c;
  }

  //  Parsing data

  if (data.length() > 0)
  {
    //    Serial.println(data);
    Serial.println("Sinkronisasi...");
    Serial.println("");

    hasilData = "";
    aktuator = "";
    int jumlahData = data.length();

    for (i = 1; i < data.length(); i++)
    {
      // pemisahan data karakter
      if (data[i] == '#')
      {
        j++;
      }

      // eksekusi jika tanda karakter di dapatkan
      if (j == 1)
      {
        if (data[i] != '#')
        {
          hasilData = hasilData + data[i];
        }
      }

      // selesai parsing
      else if (j == 2)
      {
        aktuator = aktuator + data[i];

        if (aktuator == "#" + nomesin + "ON")
        {
          digitalWrite(OUT, LOW);
          beep1x();
          Serial.println("Kran ON");
        }
        else if (aktuator == "#" + nomesin + "OFF")
        {
          digitalWrite(BUZ, LOW);
          digitalWrite(OUT, HIGH);
        }
        else
        {
          digitalWrite(BUZ, LOW);
          digitalWrite(OUT, HIGH);
        }
      }

      else if (j >= 3)
      {
        j = 0;
        k = 0;
        break;
      }
    }

    Serial.println("");
    Serial.print("Hasil Pembacaan, ");
    Serial.print("ID : ");
    Serial.print(strID);
    Serial.println(" : ");
    Serial.println(hasilData);
    Serial.println("");
    Serial.println("Code Status : ");
    Serial.println(aktuator);
    data = "";

    // delay kran dan kembali ke tampilkan tampilan awal
    if (aktuator == "#" + nomesin + "ON")
    {
      // delay pump
      delay(delay_pump);
    }
    else
    {
      beep5x();
    }

    if (hasilData)
    {
      beep1x();
    }

    digitalWrite(OUT, HIGH);
    Serial.println("Kran OFF");
    Serial.println("Tempelkan kartu ID ...");
  }

  // pembacaan TAG kartu RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  strID = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    strID += rfid.uid.uidByte[i];
  }

  if (strID)
  {
    beep1x();
  }

  Serial.println("");
  Serial.print("ID Kartu: ");
  Serial.println(strID);
  //  val = "500"; // nilai kartu yang akan dikirim

  baca_database();
  delay(10);
}

// fungsi request data dari database
void baca_database()
{
  if (!client.connect(host, port))
  {
    Serial.println("Gagal Konek server");
    beep5x();
    return;
  }

  request_string = url;
  request_string += "?idcard=";
  request_string += strID;
  request_string += "&nomes=";
  request_string += nomesin;
  request_string += "&kode=";
  request_string += kode;
  request_string += "&aktifasi=";
  request_string += aktifasi;
  request_string += "&chipid=";
  request_string += chipID;

  Serial.print("Loading, ");
  Serial.println("Mohon Tunggu...");
  Serial.println("");
  client.print(String("GET ") + request_string + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      beep5x();
      beep1x();
      beep1x();
      return;
    }
  }
}

void beep1x()
{
  digitalWrite(BUZ, HIGH);
  delay(100);
  digitalWrite(BUZ, LOW);
  delay(100);
}

void beep5x()
{
  for (int ii = 0; ii < 5; ii++)
  {
    digitalWrite(BUZ, HIGH);
    delay(100);
    digitalWrite(BUZ, LOW);
    delay(100);
  }
}
