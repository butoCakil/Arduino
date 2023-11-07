#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// input ssid dan password wifi yang tersedia
const char *ssid     = "UPTOME";
const char *password = "kurniasandi";

// Deklarasi NTP
WiFiUDP ntp;
NTPClient waktu8266(ntp, "pool.ntp.org");

//Nama hari
String hari[7]={"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};

//Nama bulan
String bulan[12]={"Januari", "Pebruari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};

void setup() {
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

// Sinkronisasi NTP
  waktu8266.begin();
  waktu8266.setTimeOffset(25200); //zona waktu=WIB=GMT+7=7*(60*60)
}

void loop() {
  waktu8266.update();
  digitalWrite(LED_BUILTIN, LOW); //indikator esp8266 dalam keadaan ON
  //Epoch Time = dalam satuan detik dihitung setelah 1 Januari 1970 00:00:00 UT
  time_t epochTime = waktu8266.getEpochTime(); 
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  int jam = waktu8266.getHours();
  Serial.print("Jam: ");
  Serial.println(jam);  

  int menit = waktu8266.getMinutes();
  Serial.print("Menit: ");
  Serial.println(menit); 
   
  int detik = waktu8266.getSeconds();
  Serial.print("Detik: ");
  Serial.println(detik);  

  String hariseminggu = hari[waktu8266.getDay()];
  Serial.print("Hari: ");
  Serial.println(hariseminggu);    

  //Mendapatkan tanggal dari Epoch Time
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int tanggal = ptm->tm_mday;
  Serial.print("Tanggal: ");
  Serial.println(tanggal);

  int angkabulan = ptm->tm_mon+1;
  Serial.print("Bulan(Angka): ");
  Serial.println(angkabulan);

  String bulansetahun = bulan[angkabulan-1];
  Serial.print("Bulan: ");
  Serial.println(bulansetahun);

  int tahun = ptm->tm_year+1900;
  Serial.print("Tahun: ");
  Serial.println(tahun);
  Serial.println("");
  
  //Menampilkan jam sekarang
  String jamsekarang = waktu8266.getFormattedTime();
  Serial.print("Jam Sekarang= ");
  Serial.println(jamsekarang); 

  //Menampilkan tanggal sekarang
  String tanggalsekarang = String(hariseminggu)+", "+ String(tanggal)+ " " + String(bulansetahun) +" "+ String(tahun);
  Serial.print("Tanggal Sekarang= ");
  Serial.println(tanggalsekarang);

  Serial.println("");

  delay(5000);
}
