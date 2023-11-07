/*AT-MO PRODUCTION
RFID DAN RELAY
tolong jika menggunakan code ini
mohon masukan sumber seperti di bawah ini

"AT-MO PRODUCTION 
EDIT BY OGI SINATRA"

* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS/SDA: Pin 10
* RST: Pin 9

*/

#include <RFID.h>
#include <SPI.h>
#include <LiquidCrystal.h>

#define SS_PIN 10 //pin RFID yang bisa di ubah
#define RST_PIN 9 //pin RFID yang bisa di ubah

#define RELAY_ON 0
#define RELAY_OFF 1
#define RELAY_1  8   // pin yang digunakan yaitu pin 8

RFID rfid(SS_PIN,RST_PIN);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int serNum[5];

int cards[][5] = {
    // alamat card 
  {
  {136, 4, 85, 85, 140},
  {211, 86, 22, 6, 149},
  {81, 225, 200, 59, 67}
  };
  
void dataBase(){
  
  }

bool access = false;

void setup(){

    Serial.begin(9600);
    SPI.begin();
    rfid.init();
    Serial.println();
    
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("Njajal RFID Coy! :D");
}

void loop(){
    lcd.display();
    delay(500);
    //lcd.clear();
    //Show UID on serial monitor
    lcd.setCursor(0,0);
    Serial.print("Tag Kartumu :\n");
    lcd.print("Tag Kartu UID:      ");
    
    
    if(rfid.isCard()){
    
        if(rfid.readCardSerial()){
            Serial.print(rfid.serNum[0]);
            Serial.print(" ");
            Serial.print(rfid.serNum[1]);
            Serial.print(" ");
            Serial.print(rfid.serNum[2]);
            Serial.print(" ");
            Serial.print(rfid.serNum[3]);
            Serial.print(" ");
            Serial.print(rfid.serNum[4]);
            Serial.println("");
            
            for(int x = 0; x < sizeof(cards); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != cards[x][i]) {
                      access = false;
                      break;
                  } else {
                      access = true;
                  }
              }
              if(access) break;
            }
           
        }


       
        
       if(access){
          Serial.println("Berhasil\n");
          lcd.setCursor(0,1);
          lcd.print("Berhasil       ");
          } 
      
      else {
           Serial.println("Maaf, Ditolak!\n");
           lcd.setCursor(0,1);
           lcd.print("Maaf, Ditolak!");
           }        
    }
    
    rfid.halt();
}
