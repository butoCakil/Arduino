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
int i, j, k, l, m, n, berhasilBaca;
unsigned long timeout;
String dataMasuk, hasilData[100];
boolean parsing = false;
boolean berhenti = false;
int loadInfo = 0;
int per;
String save_id[100];
String save_nokartu[100];
String save_nama[100];

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

  //load database
  while(berhenti == false){
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
  
   if(berhenti == true){break;}
            //baca laporan bacascan
            Serial.println("Memuat database ...");
          
            String url = "/kirimkartu/loaddatabaseguru/info.php";
              
            client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                             "Connection: close\r\n\r\n");
              
             timeout = millis();
             while (client.available() == 0) {
                  if (millis() - timeout > 5000) {
                    Serial.println(">>> Client Timeout !");
                    client.stop();
                    return;
                  }
             }
       
             while (client.available() > 0) {
                  String line = client.readStringUntil('\r');
  //                char c = client.read();
  //                dataMasuk += c;
                  dataMasuk += line;
                  Serial.println(per);
                  per++;

                  if(per > 1000){
                    Serial.println(">>> database Timeout !");
                    return;
                  }
                        
                  if (dataMasuk) {
                    parsing = true;
                  }
            
                  if(parsing == true){
                    parsingData();
  //                  Serial.println(hasilData[k]);
                  }
             }
  //        }
  }

  Serial.println("Load database selesai");
  delay(1000);
}

void loop() {

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
  hasilData[100];
  for (i = 1; i < dataMasuk.length(); i++) {
    //pengecekan tiap karakter dengan karakter (#)
    if (i >= dataMasuk.length()){
          berhenti = true;
          parsing = false;
          dataMasuk = "";
          per=0;
          Serial.println("SELESAI PARSING (i)");
          break;
        }
     
    if (dataMasuk[i] == '%')
    {
      //increment variabel j, digunakan untuk merubah index array penampung
      j++;
//      Serial.print(j);
    }
    else if(dataMasuk[i] == '$'){
      k++;
      n++;
    }
    else if(dataMasuk[i] == '#'){
//      l++;
      n = 0;
      m = 0;
    }
    else if(dataMasuk[i] == ','){
      m++;
    }
    else
    {

      if (j == 1) {
        //proses tampung data saat pengecekan karakter selesai.
        if (n == 1 && m < 1){
          save_id[k] = save_id[k] + dataMasuk[i];
        }
        if (m == 1){
          save_nokartu[k] = save_nokartu[k] + dataMasuk[i];
        }
        else if (m == 2){
          save_nama[k] = save_nama[k] + dataMasuk[i];
        }
        
//        hasilData[k] = save_id[k] + "-" + save_nama[k] +  "-" + save_info[k];
//        Serial.println(hasilData[k]);
        Serial.println("No. ");
        Serial.println(save_id[k]);
        Serial.println("ID :");
        Serial.println(save_nokartu[k]);
        Serial.println("Nama :");
        Serial.println(save_nama[k]);
        
        if (!hasilData) {
          Serial.println("GAGAL PARSING");
        }
      }
      else if (j >= 2) {
        j = 0;
        parsing = false;
        berhenti = true;
        dataMasuk = "";
        per = 0;
        Serial.println("SELESAI PARSING");
        break;
      }
    }
  }
}
