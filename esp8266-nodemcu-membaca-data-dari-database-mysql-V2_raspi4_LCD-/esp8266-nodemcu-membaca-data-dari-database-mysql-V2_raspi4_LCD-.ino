#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>

WiFiClient client;
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 2
#define RST_PIN 0
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//konfigurasi
String aktifasi = "e807f1fcf82d132f9bb018ca6738a19f";
String nomesin = "2206002";
String kode = "AIRGALON";
const char* host = "10.16.1.52";
const char* wifi = "UPTOME";
const char* pass = "kurniasandi";

String strID;
String data;
String request_string;
String hasilData;
String dataMasuk;
String aktuator;
int i, j, k = 0;

byte progresbar[8] = {
  B11111, 
  B00000, 
  B11111, 
  B11111, 
  B11111, 
  B11111, 
  B00000, 
  B11111
};

byte progresbar_[8] = {
  B00000, 
  B00000, 
  B01110, 
  B11111, 
  B11111, 
  B01110, 
  B00000, 
  B00000
};

byte progresbar_0[8] = {
  B11111, 
  B10000, 
  B10011, 
  B10111, 
  B10111, 
  B10011, 
  B10000, 
  B11111
};

byte progresbar_100[8] = {
  B11111, 
  B00001, 
  B11001, 
  B11101, 
  B11101, 
  B11001, 
  B00001, 
  B11111
};

HTTPClient http;

void setup() {
  Serial.begin(9600);
  
  pinMode(D0, OUTPUT);
  pinMode(D8, OUTPUT);
  digitalWrite(D0, LOW);
  digitalWrite(D8, LOW);

  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, progresbar);
  lcd.createChar(1, progresbar_0);
  lcd.createChar(2, progresbar_100);
  lcd.createChar(3, progresbar_);
  lcd.setCursor(4, 0);
  lcd.print(nomesin);
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print(">> ESE Project >>");
  lcd.setCursor(0, 1);      
  lcd.print("Mami Mantap - v3");
  delay(2000);
  
  WiFi.disconnect();
  WiFi.begin(wifi, pass);

  delay(100);
  Serial.println("");
  Serial.println("Menyambungkan Wifi..");

// Tampilan LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">> Konek ke Wifi   ");
// ------------

    while (((WiFi.status() != WL_CONNECTED))){
      Serial.print(".");
// Tampilan LCD      
      int a;
      lcd.setCursor(a, 1);   
      lcd.write(byte(3));
      a++;
// ------------

      delay(300);
    }
    
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("");
  Serial.println("Koneksi Berhasil!");
  Serial.println("");
  Serial.println("Tempelkan kartu ID ...");

// Tampilan LCD
  lcd.clear();
  lcd.setCursor(0, 0);      
  lcd.print("> Ready Bosku !!");
  lcd.setCursor(0, 1);
  lcd.print("Tempelkan Kartu!");
// ------------
}

void loop() { 

//  Baca Kartu
   // pembacaan TAG kartu RFID
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return;
        
    baca_kartu();
    delay(100);
    
//  Konek Database
    baca_database();
    delay(100);
    
//  Parsing
    parsing();
}

void baca_kartu(){

    strID = "";
    for(byte i=0; i< rfid.uid.size; i++){
        strID += rfid.uid.uidByte[i];
    }

    lcd.clear();
    lcd.setCursor(0, 0);      
    lcd.print("ID Kartu Terbaca");
    lcd.setCursor(0, 1);
    lcd.print(strID);
}

// fungsi request data dari database
void baca_database()
{
   if (!client.connect(host,80)) {
      lcd.clear();
      lcd.setCursor(0, 0);      
      lcd.print("Gagal Konek !!");

      for(int l = 0; l < 3; l++){
        // Buzzer
        buzz();
      }
      
      return;
    }
    
    // Buzzer
    buzz();

    request_string = "/mamimantap/apps/tag.php";
    request_string += "?idcard=";
    request_string += strID;
    request_string += "&nomes=";
    request_string += nomesin;
    request_string += "&kode=";
    request_string += kode;
    request_string += "&aktifasi=";
    request_string += aktifasi;

    // Tampila LCD
    lcd.clear();
    lcd.setCursor(0, 0);      
    lcd.print(">> Loading       ");
    lcd.setCursor(0, 1);
    lcd.print("Sabar ya, Tunggu");
    // -----------
    
    client.print(String("GET ") + request_string + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        lcd.clear();
        lcd.setCursor(0, 0);      
        lcd.print("Ada Kesalahan!");
        lcd.setCursor(0, 1);
        lcd.print("Koneksi Timeout");

        // Buzzer
        buzz();buzz();buzz();
        client.stop();
        return;
      }
    }
} 

void buzz(){
    // Buzzer TAG ON
    digitalWrite(D0,HIGH);
    delay(100);
    // Buzzer TAG OFF
    digitalWrite(D0,LOW);
    delay(100);
}

void parsing(){
  //  Parsing data
  while (client.available() > 0){
    delay(0.05);
    char c = client.read();
    data+=c;
  }
  
  if (data.length()>0){

      lcd.setCursor(0, 0);      
      lcd.print(">> Loading      ");
      lcd.setCursor(0, 1);
      lcd.print(">> Verifikasi ID");
      
      hasilData = "";
      aktuator = "";
      int jumlahData = data.length();
      int persenData;
      int load; 
      
      for(i = 1; i < data.length(); i++){

    // pemisahan data karakter
        if (data[i] == '#'){
          j++;
        }

    // eksekusi jika tanda karakter di dapatkan
        if(j==1){
          if(data[i] != '#')
          {
            hasilData = hasilData + data[i];
            persenData = ((i-193)/0.69);

              if(persenData < 0){
                  persenData = 0;
              }

            lcd.setCursor(12, 0);
            lcd.print(persenData);
            lcd.setCursor(15, 0);      
            lcd.print("%");
            delay(0.01);
          }
        }
        
        else if (j==2) {
          aktuator = aktuator + data[i];
          persenData = ((i-193)/0.69);
          
          if (persenData > 100) {
              persenData = 100;
            }

          lcd.setCursor(12, 0);
          lcd.print(persenData);
          
          if(aktuator == "#" + nomesin + "ON"){
            digitalWrite(D0,HIGH);
            digitalWrite(D8, HIGH);
          } else if(aktuator == "#" + nomesin + "OFF"){
            digitalWrite(D0,LOW);  
            digitalWrite(D8, LOW);
          } else {
            digitalWrite(D0,LOW);
            digitalWrite(D8, LOW);  
          }
          
          delay(0.01);
        }

        else if (j>=3){          
          j=0;
          k=0;
          break;
        }   

            if(persenData <= 6){load = 0;}
            else if(persenData <= 12){load = 1;}
            else if(persenData <= 18){load = 2;}
            else if(persenData <= 24){load = 3;}
            else if(persenData <= 30){load = 4;}
            else if(persenData <= 36){load = 5;}
            else if(persenData <= 42){load = 6;}
            else if(persenData <= 48){load = 7;}
            else if(persenData <= 56){load = 8;}
            else if(persenData <= 60){load = 9;}
            else if(persenData <= 66){load = 10;}
            else if(persenData <= 72){load = 11;}
            else if(persenData <= 78){load = 12;}
            else if(persenData <= 84){load = 13;}
            else if(persenData <= 90){load = 14;}
            else if(persenData <= 100){load = 15;}
            else {load = 0;}
            
            lcd.setCursor(load, 1);

            if(load == 0){lcd.write(byte(1));}
            else if(load > 14){lcd.write(byte(2));}
            else {lcd.write(byte(0));}            
            
        
      }

// waktu untuk kran menyala
    if(aktuator == ("#" + nomesin + "ON")){
      
      lcd.setCursor(0, 0);      
      lcd.print(">> Berhasil!     ");
      lcd.setCursor(0, 1);      
      lcd.print("Tunggu smp slsai");
      
      buzz(); buzz();
      
      delay(5000);
    } else {
      
      lcd.setCursor(0, 0);      
      lcd.print(">> Gagal Akses! ");
      lcd.setCursor(0, 1);      
      lcd.print("Hubungi Admin   ");

      for(int h = 0;h < 5;h++){
        buzz();
      }
      
      delay(1000);
    }

    //    selesai
    data="";
    
    digitalWrite(D8, LOW);
     
    lcd.clear();
    lcd.setCursor(0, 0);      
    lcd.print(hasilData);

    if(hasilData){
      buzz(); 
      delay(2000);
      buzz(); 
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);      
    lcd.print("  Terima        ");
    lcd.setCursor(0, 1);      
    lcd.print("        kasih!  ");
    
    // delay kembali ke tampilkan tampilan awal
    delay(1500);
  
    lcd.clear();
    lcd.setCursor(0, 0);      
    lcd.print(">> Minuman Siap!");
    lcd.setCursor(0, 1);
    lcd.print("Tempelkan Kartu!");

    digitalWrite(D0,HIGH);
    delay(500);
    buzz(); 
    
  } 
}
