#include <SPI.h> //library serial parallel interface
#include <Wire.h> //library untuk wire i2c
#include <RFID.h> //library RFID
#include <LiquidCrystal_PCF8574.h> //library penampil LCD
 
LiquidCrystal_PCF8574 lcd(0x3f); //0x3f dapat dicari dengan i2c scanner
 
#define sda 10 //Pin Serialdata (SDA)
#define rst 9 //pin Reset
 
RFID rfid(sda,rst);
 
void setup(){
    Serial.begin(9600); //baud komunikasi serial monitor
    lcd.begin(16,2); //ukuran LCD 16 x 2
    lcd.setBacklight(255); //menghidupkan lampu latar LCD
    SPI.begin(); //Prosedur antarmuka SPI
    rfid.init(); //Memulai inialisasi module RFID
 
   lcd.setCursor (0,0);
  lcd.print("***RFID RF522***");
  lcd.setCursor (0,1);
  lcd.print("-nyebarilmu.com-");
  delay (4000);
  lcd.clear();
}
 
void loop(){
  lcd.setCursor (0,0);
  lcd.print(" -yuk Scan RFID-");
  lcd.setCursor (0,1);
  lcd.print("Catat IDnya!");
 
if(rfid.isCard()){
if(rfid.readCardSerial()){
   lcd.setCursor (0,0);
   lcd.print("ID terbaca :    ");
   
   lcd.setCursor (0,1);
   Serial.print(rfid.serNum[0]); //serial no.1
   Serial.print(" ");
   lcd.print(rfid.serNum[0]);
   
   Serial.print(rfid.serNum[1]); //serial no.2
   Serial.print(" ");
   lcd.print(rfid.serNum[1]);
   
   Serial.print(rfid.serNum[2]); //serial no.3
   Serial.print(" ");
   lcd.print(rfid.serNum[2]);
    
   Serial.print(rfid.serNum[3]); //serial no.4
   Serial.print(" ");
   lcd.print(rfid.serNum[3]);
   
   Serial.print(rfid.serNum[4]); //serial no.5
   Serial.println("");
   lcd.print(rfid.serNum[4]);
 
   delay(10000);
   lcd.clear(); 
   }  }   
        }
 
