/* Sensor ultrasonic 3 wire dengan setting jarak*/
#include <NewPing.h>
//#include <HCSR04.h>
const int led = 1;
const int valve = 0;
unsigned long interval=100;         //Delay Blink LED
unsigned long previousMillis=0;
unsigned long intervalA=100;          //Delay Blink - ON 
unsigned long intervalB=1200;         //Delay Blink - OFF 
unsigned long tunda=1000; 
bool ledState = false;
bool stateLoad  = false;

// Initialize sensor that uses digital pins 4-trig and 3-echo.
//UltraSonicDistanceSensor distanceSensor(4, 3); 

#define TRIGGER_PIN 4
#define ECHO_PIN 4
#define MAX_DISTANCE 200 // max distance the sensor will return
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // declare a NewPing object

void setup () {
    // We initialize serial connection so that we could print values from sensor.
    Serial.begin(9600);  
    pinMode(valve, OUTPUT);
    pinMode(led, OUTPUT);
    digitalWrite(led, ledState);
}

void loop () {
    int sensorValue = analogRead(3);
    float voltage = sensorValue * (25.0 / 1023.0);
    int setJarak = voltage;
    int setJrknol = setJarak - 10;
    if (setJrknol <3){setJrknol = 3;}
    //jarak = distanceSensor.measureDistanceCm();
    delay(50);
    int uS = sonar.ping(); 
    int jarak = uS / US_ROUNDTRIP_CM;
    
    if(jarak > setJrknol & jarak < setJarak)
      {
        
        digitalWrite(valve, LOW);   //Relay ON - Rangkaian relay Aktif LOW
        digitalWrite(led, HIGH);  //LED ON saat relay ON
        stateLoad = true;
        previousMillis = millis();
      }
     else {
          digitalWrite(valve, HIGH);  //Relay OFF - Rangkaian relay Aktif LOW
          //blink LED//
          unsigned long currentMillis = millis();
          
          if(stateLoad == true){
              digitalWrite(valve, LOW);
              digitalWrite(led, HIGH);
              //delay untuk relay on setelah tidak mendeteksi, set ke false   
              if((currentMillis-previousMillis)>=tunda) 
              {stateLoad=false; digitalWrite(led, LOW);}
              } 
          
          if(stateLoad == false){
            if ((unsigned long)(currentMillis - previousMillis) >= interval) {
              if(interval == intervalA){digitalWrite(led, LOW); interval=intervalB;}
              else {digitalWrite(led, HIGH); interval=intervalA;}
            previousMillis = millis();
            digitalWrite(valve, HIGH);
            }
          }  
  }
}
