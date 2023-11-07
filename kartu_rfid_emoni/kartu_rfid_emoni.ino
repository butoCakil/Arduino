#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 4
#define RST_PIN 5
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
String request_string;
const char* host = "192.168.1.4"; // ip computer yang di install xampp (Windows + R ---> CMD ---->ketik ipconfig
HTTPClient http;
void setup() {
  // put your setup code here, to run once:
    WiFi.disconnect();
   WiFi.begin("KOST_RAMA","bayardulu500");
    while ((!(WiFi.status() == WL_CONNECTED))){
      delay(300);
    }
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("I am waiting for card...");
}
String strID,val, data;
void loop() {
  // put your main code here, to run repeatedly:
  while (client.available()>0){
    delay(10);
    char c = client.read();
    data+=c;
  }
  if (data.length()>0){
    Serial.println(data);
    data="";
  }

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;
  // Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Serial.println(rfid.PICC_GetTypeName(piccType));
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
//id kartu dan yang akan dikirim ke database
  strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }

  strID.toUpperCase();
  Serial.print("Tap card key: ");
  Serial.println(strID);
  val = "500"; // nilai kartu yang akan dikirim
//  tambah_database();
  baca_database();
  delay(1000);

}
void tambah_database()
{
   if (!client.connect(host,80)) {
      Serial.println("Gagal Konek");
      return;
    }
    request_string = "/rfid/rfidadd.php?idcard=";
    request_string += strID;
    request_string += "&val=";
    request_string += val;
    Serial.print("requesting URL: ");
    Serial.println(request_string);
    client.print(String("GET ") + request_string + "HTTP/1.1rn" + "Host: " + host + "rn" + "Connection: closernrn");
  
    unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
}
void baca_database()
{
   if (!client.connect(host,80)) {
      Serial.println("Gagal Konek");
      return;
    }
    request_string = "/rfid/rfidread.php?idcard=";
    request_string += strID;
    Serial.print("requesting URL: ");
    Serial.print(request_string);
    client.print(String("GET ") + request_string + " HTTP/1.1rn" + "Host: " + host + "rn" + "Connection: closernrn");
  
    unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
} 
