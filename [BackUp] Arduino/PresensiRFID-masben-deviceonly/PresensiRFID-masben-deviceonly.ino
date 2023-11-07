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
//#define BTN_PIN 5  //D1

//sediakan variabel untuk RFID
#define SDA_PIN 2  //D4
#define RST_PIN 0  //D3

String IDTAG;
//String hasilTAG;
int i, j, berhasilBaca;
unsigned long timeout;
String dataMasuk, hasilData;
boolean parsing=false;
int loadInfo;

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
  
  while(WiFi.status() != WL_CONNECTED)
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
  //pinMode(BTN_PIN, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Tempelkan Kartu RFID Anda ke Reader");
  Serial.println();
}

void loop() {
  WiFiClient client;

    if (!client.connect(host, port)) {
      Serial.println("Menyambungkan lagi...");
      i = 0;
      return;
    } 
    if(i==0) {
      Serial.println("Koneksi OK");
      i = 1;
    }
    
  berhasilBaca = bacaTag();

  if(berhasilBaca){
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Nomor ID Kartu :" + IDTAG);
      String apiUrl = "/kirimkartu/add/" + IDTAG;
    
      client.print(String("GET ") + apiUrl + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
  }

      if(millis() - loadInfo > 100){
          loadInfo = millis();
        
          //baca laporan bacascan
          Serial.println("Memuat info ...");
        
          String url = "/home/tagbaca/info";
            
          client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                      "Host: " + host + "\r\n" +
                           "Connection: close\r\n\r\n");
            
           timeout = millis();
           while (client.available() == 0) {
                if (millis() - timeout > 10000) {
                  Serial.println(">>> Client Timeout !");
                  client.stop();
                  return;
                }
           }
     
           while (client.available()) {
                String line = client.readStringUntil('\r');
                dataMasuk += line;
//                Serial.println(line);
                      
                if (dataMasuk) {
                  parsing = true;
                }
          
                if(parsing == true){
                  parsingData();
                  Serial.println(hasilData);
                }
           }
           
      Serial.println();
      Serial.println("Koneksi Selesai");
      Serial.println();
      digitalWrite(LED_BUILTIN, HIGH);
      }
  
}


int bacaTag(){
  if(! mfrc522.PICC_IsNewCardPresent())
     return 0;

  if(! mfrc522.PICC_ReadCardSerial())
     return 0;

  IDTAG = "";
  for(byte i=0; i<mfrc522.uid.size; i++){
    IDTAG += mfrc522.uid.uidByte[i];
  }

  return 1;
}

void parsingData(){
  hasilData="";
  for(i=1;i<dataMasuk.length();i++){
      //pengecekan tiap karakter dengan karakter (#)
      if (dataMasuk[i] == '#')
      {
          //increment variabel j, digunakan untuk merubah index array penampung
          j++;       
      }
      else
      {
          //proses tampung data saat pengecekan karakter selesai.
          if(j==1){
            hasilData = hasilData + dataMasuk[i];
            if(!hasilData){
                Serial.println("GAGAL PARSING");
            }
          }
          else if (j>=2){
            j=0;
            parsing=false;
            dataMasuk="";
            Serial.println("SELESAI PARSING");
            break;
          }
      }
  }
}
