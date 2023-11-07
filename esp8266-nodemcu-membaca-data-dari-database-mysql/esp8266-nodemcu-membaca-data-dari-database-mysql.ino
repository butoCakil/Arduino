/*
 * visit www.anakkendali.com
 * 2018
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 2
#define RST_PIN 0
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
String strID, val;
String data;
String request_string;
String hasilData;
String dataMasuk;
int i, j;
const char* host = "10.16.1.118"; // ip computer yang di install xampp (Windows + R ---> CMD ---->ketik ipconfig
HTTPClient http;
void setup() {
  Serial.begin(9600);
  
  WiFi.disconnect();
  WiFi.begin("UPTOME","kurniasandi");
    
    while (((WiFi.status() != WL_CONNECTED))){
      Serial.print(".");
      delay(300);
    }
    
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("");
  Serial.println("Tempelkan kartu ID ...");
}

void loop() {

  while (client.available()>0){
    delay(10);
    char c = client.read();
    data+=c;
  } 

//  Parsing data

  if (data.length()>0){
    Serial.println(data);
      dataMasuk = data;
      hasilData = "";
      int jumlahData = dataMasuk.length();
      
      for(i=1; i<dataMasuk.length(); i++){

// pemisahan data karakter
        if (dataMasuk[i] == '#'){
          j++;
        }

// eksekusi jika tanda karakter di dapatkan
        if(j==1){
          if(dataMasuk[i] != '#')
          {
            hasilData = hasilData + dataMasuk[i];
            int persenData = 100 - (dataMasuk.length() - i);
            
            Serial.print("Loading ");
            Serial.print(persenData);
            Serial.println(" %");
          }
        }
// selesai parsing
        else if (j>=2) {
          Serial.println("SELESAI PARSING");
          j=0;
          break;
        }
      }

    Serial.println("");
    Serial.println("Hasil Parsing");
    Serial.println(hasilData);
    data="";

// delay tampilkan tampilan awal
    delay(3000);
    Serial.println("Tempelkan kartu ID ...");
  } 

// pembacaan TAG kartu RFID
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
  Serial.println("");
  Serial.print("ID Kartu: ");
  Serial.println(strID);
  val = "500"; // nilai kartu yang akan dikirim

  baca_database();
  delay(1000);
}

// fungsi request data dari database
void baca_database()
{
   if (!client.connect(host,80)) {
      Serial.println("Gagal Konek");
      return;
    }
    
    request_string = "/emanpo/apps/bacaDB.php?idcard=";
    request_string += strID;
    Serial.println("Loading...");
    Serial.println("Mohon Tunggu...");
    client.print(String("GET ") + request_string + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
} 
