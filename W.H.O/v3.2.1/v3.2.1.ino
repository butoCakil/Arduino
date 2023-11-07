/* Sensor ultrasonic 3 wire dengan setting jarak*/
#include <NewPing.h>
//#include <HCSR04.h>

unsigned long interval=500;         //Delay Blink LED
unsigned long previousMillis=0;

bool ledState = false;

// Initialize sensor that uses digital pins 4-trig and 3-echo.
//UltraSonicDistanceSensor distanceSensor(4, 3); 

#define TRIGGER_PIN 4
#define ECHO_PIN 4
#define MAX_DISTANCE 200 // max distance the sensor will return
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // declare a NewPing object

void setup () {
    // We initialize serial connection so that we could print values from sensor.
    Serial.begin(9600);  
    pinMode(2, OUTPUT);
    pinMode(1, OUTPUT);
    digitalWrite(1, ledState);
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
        
        digitalWrite(2, LOW);   //Relay ON - Rangkaian relay Aktif LOW
        digitalWrite(1, HIGH);  //LED ON saat relay ON
        delay(1000);
      }
     else {
          digitalWrite(2, HIGH);  //Relay OFF - Rangkaian relay Aktif LOW
          //blink LED//
          unsigned long currentMillis = millis();
          if ((unsigned long)(currentMillis - previousMillis) >= interval) {
          ledState = !ledState;
          digitalWrite(1, ledState);
          previousMillis = millis();
          }
  }
}
