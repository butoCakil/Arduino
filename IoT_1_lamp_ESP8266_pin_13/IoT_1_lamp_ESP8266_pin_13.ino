// Setting Device - User //
String akses = "iot0001";
String aktifasi = "e807f1fcf82d132f9bb018ca6738a19f";

// Setting Koneksi //
const char *ssid = "INTRUKTUR-TAV-2.4G";
const char *pass = "skanebabisa";

// Setting url //
const char *host = "bennybendz.com";
String url = "/iotcontrol/apps/feedback.php";
int port = 80;

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
#include <SPI.h>

#define BUZ D8
#define OUT D0
#define SS_PIN 2
#define RST_PIN 0
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
//  pinMode(D2, INPUT_PULLUP);

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

    int jumlahData = data.length();

    Serial.println("");
    Serial.print("Hasil Pembacaan, ");
    Serial.println(data);
    data = "";
  }

  delay(10);
}

// fungsi request data dari database
void baca_database()
{
  if (!client.connect(host, port))
  {
    Serial.println("Gagal Konek server");
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
      return;
    }
  }
}
