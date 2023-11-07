#include <HCSR04.h>

unsigned long interval=100;         //Delay Blink - nilai awal 
unsigned long intervalBlinkOn=500;  //Delay Blink saat power full on
unsigned long intervalA=100;        //Delay Blink - ON mode hemat
unsigned long intervalB=2900;       //Delay Blink - OFF mode hemat
unsigned long tunda=1000;           //Delay Output
unsigned long previousMillis=0;
unsigned long currentMillis=0;
unsigned long standBy=15000;        //waktu menuju sleep - mode hemat
unsigned long terakhirLoad;

const int load = 0;
const int valveToggle = 2;
const int led = 1;
bool ledState = false;
bool statePwr = true;
bool stateLoad = false;
int tombolState = 0;
int jarak;

// Initialize sensor, pins 4-trig dan pin 3-echo.
UltraSonicDistanceSensor distanceSensor(4, 3);  

void setup () {
    // We initialize serial connection so that we could print values from sensor.
    Serial.begin(9600);  
    pinMode(led, OUTPUT);
    pinMode(load, OUTPUT);
    digitalWrite(led, ledState);
    digitalWrite(load, HIGH);
}

void loop () {
    //Mode Aktif Penuh
    if(statePwr==true){
            //aktifkan daya penuh
            digitalWrite(load, HIGH);

            //membaca jarak
            jarak = distanceSensor.measureDistanceCm();
            //jarak yang akan dideteksi
            if(jarak > 6 & jarak <= 10)
              {
                //set pin 2, sebagai Output
                pinMode(valveToggle, OUTPUT);
                //Relay ON - Rangkaian relay Aktif LOW
                digitalWrite(valveToggle, LOW);   
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
                            digitalWrite(valveToggle, LOW);
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
                                    
                               digitalWrite(valveToggle, HIGH);  //Relay OFF
                               }

                        //Timming ke Mode Sleep (Standby)
                        if((currentMillis - terakhirLoad) >= standBy){
                          //matikan daya berlebih - masuk mode hemat
                          statePwr =! statePwr;         //set false
                          digitalWrite(load, LOW);
                          digitalWrite(led, LOW);
                          digitalWrite(valveToggle, LOW);
                          //set output ke relay sebagai input push button
                          pinMode(valveToggle, INPUT_PULLUP); 
                          }
                        }
              }
     }

     //Mode Standby
     if(statePwr==false){

        //menyimpat hasil pembacaan button
        tombolState = digitalRead(valveToggle);

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
        jarak = distanceSensor.measureDistanceCm();
        if(jarak > 6 & jarak <= 10){
          statePwr = !statePwr;     //set true
          stateLoad = !stateLoad;   //set false
        }
     }
}
