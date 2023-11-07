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
String aktuator;
int i, j;

String nomesin = "2206001";
String kode = "AIRGALON";
const char* host = "10.16.1.118"; // ip computer yang di install xampp (Windows + R ---> CMD ---->ketik ipconfig
HTTPClient http;
void setup() {
  Serial.begin(9600);
  
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  
  WiFi.disconnect();
  WiFi.begin("UPTOME","kurniasandi");

  delay(100);
  Serial.println("");
  Serial.println("Menyambungkan Wifi..");
    
    while (((WiFi.status() != WL_CONNECTED))){
      Serial.print(".");
      delay(300);
    }
    
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("");
  Serial.println("Koneksi Berhasil!");
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
      hasilData = "";
      aktuator = "";
      int jumlahData = data.length();
      
      for(i=1; i<data.length(); i++){

// pemisahan data karakter
        if (data[i] == '#'){
          j++;
        }

// eksekusi jika tanda karakter di dapatkan
        if(j==1){
          if(data[i] != '#')
          {
            hasilData = hasilData + data[i];
            int persenData = 100 - (data.length() - i);
            
            Serial.print("Loading ");
            Serial.print(persenData);
            Serial.println(" %");
          }
        }
        
// selesai parsing
        else if (j==2) {
          aktuator = aktuator + data[i];
          int persenData = 100 - (data.length() - i);
          Serial.print("Loading ");
          Serial.print(persenData);
          Serial.println(" %");
          
          if(aktuator == "#2206001ON"){
            digitalWrite(D0,HIGH);
            Serial.println("Kran ON");
          } else if(aktuator == "#2206001OFF"){
            digitalWrite(D0,LOW);  
          } else {
            digitalWrite(D0,LOW);  
          }
        }

        else if (j>=3){
          Serial.println("Selesai.. ");
          j=0;
          break;
        }
      }

    Serial.println("");
    Serial.println("Hasil Parsing");
    Serial.println(hasilData);
    Serial.println("");
    Serial.println("Hasil aktuator");
    Serial.println(aktuator);
    data="";

// delay kran dan kembali ke tampilkan tampilan awal

    if(aktuator == "#2206001ON"){
      delay(5000);
    }
    
    digitalWrite(D0,LOW);  
    Serial.println("Kran OFF");
    Serial.println("Tempelkan kartu ID ...");
  } 

// pembacaan TAG kartu RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

   strID = "";
   for(byte i=0; i< rfid.uid.size; i++){
      strID += rfid.uid.uidByte[i];
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
   if (!client.connect(host,80)) {
      Serial.println("Gagal Konek");
      return;
    }
    
    request_string = "/mamimantap/apps/tag.php";
    request_string += "?idcard=";
    request_string += strID;
    request_string += "&nomes=";
    request_string += nomesin;
    request_string += "&kode=";
    request_string += kode;

// Buzzer TAG ON
    digitalWrite(D0,HIGH);
    delay(200);
    
    Serial.print("Loading, ");
    Serial.println("Mohon Tunggu...");
    client.print(String("GET ") + request_string + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

// Buzzer TAG OFF
    digitalWrite(D0,LOW);
  
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
} 
