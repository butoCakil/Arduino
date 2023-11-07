#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>

// input ssid dan password wifi yang tersedia
const char *ssid     = "UPTOME";
const char *password = "kurniasandi";

// text
String teks = "Menjajal Running text LCD I2C... ";
int indeks = 0;
int status = 0;

// LCD config
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Deklarasi NTP
WiFiUDP ntp;
NTPClient waktu8266(ntp, "pool.ntp.org");

//Nama hari
String hari[7]={"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};

//Nama bulan
String bulan[12]={"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "Septembr", "Oktober", "November", "Desember"};

void setup() {
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  pinMode(LED_BUILTIN, OUTPUT); // led built in
  Serial.begin(115200);
  
  // Konek ke Wi-Fi
  Serial.print("Menyambungkan ke  ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Terhubung!");
// Sinkronisasi NTP
  waktu8266.begin();
  waktu8266.setTimeOffset(25200); //zona waktu=WIB=GMT+7=7*(60*60)

// LCD inisialisasi  
  lcd.begin(16,2);
  lcd.init();
 
// Nyalakan backlight
  lcd.backlight();

// Welcome
  lcd.setCursor(0, 0);
  lcd.print(">> ESE Project >");
  lcd.setCursor(0, 1);      
  lcd.print("JAM NTP - IoT >>");

  delay(3000);
  lcd.clear();
}

void loop() {
  waktu8266.update();
  digitalWrite(LED_BUILTIN, LOW); //indikator esp8266 dalam keadaan ON
  //Epoch Time = dalam satuan detik dihitung setelah 1 Januari 1970 00:00:00 UT
  time_t epochTime = waktu8266.getEpochTime(); 
//   Serial.print("Epoch Time: ");
//   Serial.println(epochTime);
  
  int jam = waktu8266.getHours();
//   Serial.print("Jam: ");
//   Serial.println(jam);  

  int menit = waktu8266.getMinutes();
//   Serial.print("Menit: ");
//   Serial.println(menit); 
   
  int detik = waktu8266.getSeconds();
//   Serial.print("Detik: ");
//   Serial.println(detik);  

  String hariseminggu = hari[waktu8266.getDay()];
//   Serial.print("Hari: ");
//   Serial.println(hariseminggu);    

  //Mendapatkan tanggal dari Epoch Time
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int tanggal = ptm->tm_mday;
//   Serial.print("Tanggal: ");
//   Serial.println(tanggal);

  int angkabulan = ptm->tm_mon+1;
//   Serial.print("Bulan(Angka): ");
//   Serial.println(angkabulan);

  String bulansetahun = bulan[angkabulan-1];
//   Serial.print("Bulan: ");
//   Serial.println(bulansetahun);

  int tahun = ptm->tm_year+1900;
//   Serial.print("Tahun: ");
//   Serial.println(tahun);
//   Serial.println("");
  
  //Menampilkan jam sekarang
  String jamsekarang = waktu8266.getFormattedTime();
//   Serial.print("Jam Sekarang= ");
//   Serial.println(jamsekarang); 

  //Menampilkan tanggal sekarang
//  String haritanggalsekarang = String(hariseminggu) +", "+ String(tanggal)+ " " + String(bulansetahun) +" "+ String(tahun);
  String tanggalsekarang = String(tanggal)+ " " + String(bulansetahun) +" "+ String(tahun);

//   Serial.print("Tanggal Sekarang= ");
//   Serial.println(tanggalsekarang);

//   Serial.println("");

// ------------- FORMAT 1 -----------------------
// --------  hari, HH/bb/TTTT -------------------
// --------      JJ:MM:DD     -------------------

//  lcd.setCursor(0, 0);
//  lcd.print(hariseminggu);
//  lcd.setCursor(6, 0);
//  lcd.print(tanggal);
//  lcd.setCursor(8, 0);
//  lcd.print("-");
//
//  if(angkabulan < 10){
//    lcd.setCursor(9, 0);
//    lcd.print("0");  
//    lcd.setCursor(10, 0);
//    lcd.print(angkabulan);  
//  } else {
//    lcd.setCursor(9, 0);
//    lcd.print(angkabulan);
//  }
//
//  lcd.setCursor(11, 0);
//  lcd.print("-");
//  lcd.setCursor(12, 0);
//  lcd.print(tahun);
//  
//  lcd.setCursor(4, 1);      
//  lcd.print(jamsekarang);

// ------------- ./ FORMAT 1 -----------------------

//--------------- FORMAT 2 ------------------------
// --------  HH/bb/TTTT     -----------------------
// --------  JJ:MM:DD hari  -----------------------

  lcd.setCursor(0, 0);
  lcd.print(tanggalsekarang);

  lcd.setCursor(0, 1);      
  lcd.print(jamsekarang);
  lcd.setCursor(8, 1);
  lcd.print(", ");
  lcd.setCursor(10, 1);
  lcd.print(hariseminggu);
  
  delay(1000);
  
// ------------- ./ FORMAT 2 -----------------------

  if(menit % 5 == 0 && detik == 0){
    beep1();
    status = 1;
    lcd.clear();
  } 
}

void beep1() {
  // Buzzer TAG ON
    digitalWrite(D0,HIGH);
    delay(1000);
    // Buzzer TAG OFF
    digitalWrite(D0,LOW);
    delay(100);

    for(int i = 0; i < 3; i++){
      // Buzzer TAG ON
      digitalWrite(D0,HIGH);
      delay(100);
      // Buzzer TAG OFF
      digitalWrite(D0,LOW);
      delay(100);
    }
}

void runningtext_all(){
    for (int u = 0 ; u < 16; u++) {
      lcd.scrollDisplayLeft();
      //lcd.scrollDisplayRight();
      delay(500);
    }
}

void runningtext_1(){
   char st[17]; // String dengan maksimal 16 karakter
   st[16] = 0; // Akhir string
  
   // Bentuk string
   for (int j = 0; j < 16; j++)
      if (indeks + j < teks.length())
          st[j] = teks[indeks + j];
      else
          st[j] = 32; // Spasi
  
   // Tampilkan di baris kedua
   lcd.setCursor(0, 1);
   lcd.print(st);
  
   // Peroleh indeks berikutnya
   indeks++;
   
   if (indeks == teks.length() - 1){
     indeks = 0; 
     status == 0;
     lcd.clear();
   }
        
   delay(500);
}
