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
int incomingByte;
//String save_id[100];
//String save_nokartu[100];
//String save_nama[100];
int id[66] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66};
String no_id[66] = { "12344567890", "456768990987656", "6564456788987", "3373211410028796", "3503314608982564", "3571632202943120", "5306070811104834", "6403522809105830", "8103166405132524", "1605701711112172", "1213135103111522", "3206765610203343", "7208286405060962", "7603871011015769", "3576845809001150", "5103071301055993", "3503630201175371", "1902860110053932", "1312250409959920", "1904960106034070", "5302180411993025", "3577575909097520", "1108095208010491", "1403031910164574", "1610314501959731", "1771801306030051", "5321136808085794", "3324874907116837", "7405656809998942", "3522644802200237", "3314514207925708", "6304321809131500", "6109151007044369", "1407985204016420", "1608366711992118", "3373032612214388", "6112834608166827", "3329266502972175", "1471962108027708", "7301483011132927", "7408817108166403", "1809776209193882", "7605533005175409", "9119251902999977", "9114846108079725", "1603216001207652", "9121881403048171", "1309891006975763", "3578154408192777", "1571750610156972", "6109036806995215", "7401354801969031", "7404302603110546", "1117730509064916", "1706170506200583", "6308681512086271", "7301286103075211", "5301704104109325", "3372296011067723", "1503454401147066", "8201992511930482", "9205692002995475", "9205572701991289", "21151216", "163196895", "1761124250"};
String nama[66] =  { "Ir. Joko Widodo", "Benny Surahman", "Yeni Inka", "Ian Hidayat", "Raihan Damanik", "Jamalia Sudiati", "Bakiman Uwais", "Uda Bahuwarna Nashiruddin M.Farm", "Iriana Tina Purwanti", "Citra Hani Laksmiwati", "Paramita Raisa Maryati", "Wardi Bagas Wijaya", "Ian Maryadi", "Lala Tina Mayasari", "Pranawa Marbun M.Ak", "Ade Susanti S.Farm", "Hendra Tampubolon", "Artanto Yusuf Kurniawan", "Ida Ulya Hassanah", "Usyi Pudjiastuti", "Maryanto Haryanto", "Zelaya Nasyidah S.H.", "Lega Gunawan S.Pd", "Cahyo Karya Tamba", "Reza Tamba", "Ibrahim Budiman", "Usyi Sudiati", "Nilam Widiastuti S.T.", "Mila Ratih Kusmawati M.M.", "Lili Rahmi Mulyani", "Rina Nasyidah", "Cemeti Ivan Pradana S.H.", "Sadina Wahyuni", "Citra Suartini", "Hamima Yuniar", "Padma Sudiati", "Rika Bella Permata", "Kemal Maheswara S.Pt", "Eva Kania Rahayu M.TI.", "Prabowo Kusumo", "Jati Prayoga", "Tina Hariyah", "Capa Edi Nugroho S.IP", "Gamblang Wahyudin", "Ifa Haryanti", "Zulaikha Ratna Yolanda M.Kom.", "Rahmi Hani Rahimah", "Jaiman Sihotang M.Ak", "Kezia Anggraini", "Uli Nuraini S.IP", "Malika Umi Lestari", "Wahyu Hidayat", "Farhunnisa Halimah S.H.", "Hana Hartati", "Eja Wahyudin", "Widya Pertiwi", "Johan Kurniawan M.TI.", "Hamzah Adriansyah", "Ilyas Dwi Anggriawan", "Nasrullah Prasetya Tampubolon S.Gz", "Jagaraga Budiman", "Digdaya Laswi Tamba M.M.", "Jasmin Dian Handayani", "Ndes Ku", "Doni", "Aku"};

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
//  while(berhenti == false){
//    WiFiClient client;
//  
//    if (!client.connect(host, port)) {
//      Serial.println("Menyambungkan lagi...");
//      i = 0;
//      return;
//    }
//    if (i == 0) {
//      Serial.println("Koneksi OK");
//      i = 1;
//    }
//  
//   if(berhenti == true){break;}
//            //baca laporan bacascan
//            Serial.println("Memuat database ...");
//          
//            String url = "/kirimkartu/loaddatabaseguru/info.php";
//              
//            client.print(String("GET ") + url + " HTTP/1.1\r\n" +
//                        "Host: " + host + "\r\n" +
//                             "Connection: close\r\n\r\n");
//              
//             timeout = millis();
//             while (client.available() == 0) {
//                  if (millis() - timeout > 5000) {
//                    Serial.println(">>> Client Timeout !");
//                    client.stop();
//                    return;
//                  }
//             }
//       
//             while (client.available() > 0) {
//                  String line = client.readStringUntil('\r');
//  //                char c = client.read();
//  //                dataMasuk += c;
//                  dataMasuk += line;
//                  Serial.println(per);
//                  per++;
//
//                  if(per > 1000){
//                    Serial.println(">>> database Timeout !");
//                    return;
//                  }
//                        
//                  if (dataMasuk) {
//                    parsing = true;
//                  }
//            
//                  if(parsing == true){
//                    parsingData();
//  //                  Serial.println(hasilData[k]);
//                  }
//             }
//  //        }
//  }
//
//  Serial.println("Load database selesai");
//  delay(1000);
}

void loop() {
for(i=0;i<66;i++){
//       Serial.print(id[incomingByte] + no_id[incomingByte] + nama[incomingByte]);
       Serial.println(i+1);
       Serial.println("No. ");
       Serial.println(id[i]);
       Serial.println("ID :");
       Serial.println(no_id[i]);
       Serial.println("Nama :");
       Serial.println(nama[i]);
       Serial.println("");
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
//  hasilData[100];
//  for (i = 1; i < dataMasuk.length(); i++) {
//    //pengecekan tiap karakter dengan karakter (#)
//    if (i >= dataMasuk.length()){
//          berhenti = true;
//          parsing = false;
//          dataMasuk = "";
//          per=0;
//          Serial.println("SELESAI PARSING (i)");
//          break;
//        }
//     
//    if (dataMasuk[i] == '%')
//    {
//      //increment variabel j, digunakan untuk merubah index array penampung
//      j++;
////      Serial.print(j);
//    }
//    else if(dataMasuk[i] == '$'){
//      k++;
//      n++;
//    }
//    else if(dataMasuk[i] == '#'){
////      l++;
//      n = 0;
//      m = 0;
//    }
//    else if(dataMasuk[i] == ','){
//      m++;
//    }
//    else
//    {
//
//      if (j == 1) {
//        //proses tampung data saat pengecekan karakter selesai.
//        if (n == 1 && m < 1){
//          save_id[k] = save_id[k] + dataMasuk[i];
//        }
//        if (m == 1){
//          save_nokartu[k] = save_nokartu[k] + dataMasuk[i];
//        }
//        else if (m == 2){
//          save_nama[k] = save_nama[k] + dataMasuk[i];
//        }
//        
////        hasilData[k] = save_id[k] + "-" + save_nama[k] +  "-" + save_info[k];
////        Serial.println(hasilData[k]);
//        Serial.println("No. ");
//        Serial.println(save_id[k]);
//        Serial.println("ID :");
//        Serial.println(save_nokartu[k]);
//        Serial.println("Nama :");
//        Serial.println(save_nama[k]);
//        
//        if (!hasilData) {
//          Serial.println("GAGAL PARSING");
//        }
//      }
//      else if (j >= 2) {
//        j = 0;
//        parsing = false;
//        berhenti = true;
//        dataMasuk = "";
//        per = 0;
//        Serial.println("SELESAI PARSING");
//        break;
//      }
//    }
//  }
}
