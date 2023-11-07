#include <NewPing.h>
//ultrasonic 3 wire
 
#define TRIGGER_PIN 4
#define ECHO_PIN 4
#define MAX_DISTANCE 200 // max distance the sensor will return
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // declare a NewPing object

unsigned long interval=100;           //Delay Blink - nilai awal 
unsigned long intervalBlinkOn = 500;  //Delay Blink saat power full on
unsigned long intervalA=100;          //Delay Blink - ON mode hemat
unsigned long intervalB=2900;         //Delay Blink - OFF mode hemat
unsigned long tunda=1000;             //Delay Output
unsigned long previousMillis  = 0;
unsigned long currentMillis   = 0;
unsigned long standBy=15000;          //waktu menuju sleep - mode hemat
unsigned long terakhirLoad; 
const int load    = 0;
const int led     = 1;
const int valve   = 2;
const int tombol  = 3;
bool ledState   = false;
bool statePwr   = true;
bool stateLoad  = false;
int tombolState = 0;
int jarak, setJarak, setJrknol;

void setup () {
    // We initialize serial connection so that we could print values from sensor.
    Serial.begin(9600);  
    //Serial.begin(115200);
    pinMode(tombol,INPUT_PULLUP);
    pinMode(led, OUTPUT);
    pinMode(load, OUTPUT);
    pinMode(valve, OUTPUT);
    digitalWrite(led, ledState);
    digitalWrite(load, HIGH);
}

void bacaJarak(){
  int sensorValue = analogRead(3);
  setJarak = sensorValue * (25.0 / 1023.0);
  setJrknol = setJarak - 7;
  if (setJrknol <3){setJrknol = 3;}
  
  delay(10);
  int uS = sonar.ping(); 
  jarak = uS / US_ROUNDTRIP_CM;
}

void loop () {
    //Mode Aktif Penuh
    if(statePwr==true){
            //aktifkan daya penuh
            digitalWrite(load, HIGH);

            //membaca jarak
            bacaJarak();
            
            //jarak yang akan dideteksi
            if(jarak > 3 & jarak <= setJarak)
              {
                //Relay ON - Rangkaian relay Aktif LOW
                digitalWrite(valve, LOW);   
                //LED ON saat relay ON
                digitalWrite(led, HIGH);   
                //set status bahwa telah ada pembebanan (loading - aktifkan relay)       
                stateLoad = true;
                //mencatatan waktu terakhir jarak terdeteksi
                previousMillis = millis();
              }
             else
              {
                   //mencatat waktu sekarang
                   currentMillis = millis();
                        //Setelah mendeteksi object, mempertahankan kondisi sebelumnya
                        if(stateLoad == true){
                            digitalWrite(valve, LOW);
                            digitalWrite(led, HIGH);
                        //delay untuk relay on setelah tidak mendeteksi, set ke false   
                        if((currentMillis-previousMillis)>=tunda) {stateLoad=!stateLoad;}
                        //mencatat waktu terakhir Relay aktif  
                        terakhirLoad = millis();
                        } 

                        //Mode ketika tidak mendeteksi Object
                        if(stateLoad == false){     
                            //blink LED//
                            if ((currentMillis - previousMillis) >= intervalBlinkOn) {
                               ledState = !ledState;
                               digitalWrite(led, ledState);
                               previousMillis = millis();
                                    
                               digitalWrite(valve, HIGH);  //Relay OFF
                               }

                        //Timming ke Mode Sleep (Standby)
                        if((currentMillis - terakhirLoad) >= standBy){
                          //matikan daya berlebih - masuk mode hemat
                          statePwr =! statePwr;         //set false
                          digitalWrite(load, LOW);
                          digitalWrite(led, LOW);
                          digitalWrite(valve, LOW);
                          }
                        }
              }
     }

     //Mode Standby
     if(statePwr==false){

        //menyimpat hasil pembacaan button
        tombolState = digitalRead(tombol);

        //***LED blink 100 High - 2900 mS Low***//
        currentMillis = millis();
        if ((currentMillis - previousMillis) >= interval) {
               if(interval == intervalA){digitalWrite(led, LOW); interval=intervalB;}
               else {digitalWrite(led, HIGH); interval=intervalA;}
               previousMillis = millis();
               }
         //***//
         //Aktifkan, Jika Push Button ditekan
        if(tombolState == LOW){
          delay(50);                //delay mencegah lompatan 
          statePwr = !statePwr;     //set true
          stateLoad = !stateLoad;   //set false
          }

        //Aktifkan saat mendeteksi
        bacaJarak();
        
        if(jarak > 3 & jarak <= setJarak){
          statePwr = !statePwr;     //set true
          stateLoad = !stateLoad;   //set false
        }
     }
}
