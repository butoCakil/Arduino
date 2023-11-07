#include <HCSR04.h>

unsigned long interval=500;         //Delay Blink LED
unsigned long previousMillis=0;

bool ledState = false;

// Initialize sensor that uses digital pins 4-trig and 3-echo.
UltraSonicDistanceSensor distanceSensor(4, 3);  

void setup () {
    // We initialize serial connection so that we could print values from sensor.
    Serial.begin(9600);  
    pinMode(2, OUTPUT);
    pinMode(1, OUTPUT);
    digitalWrite(1, ledState);
}

void loop () {
    int jarak;
    jarak = distanceSensor.measureDistanceCm();
    if(jarak > 3 & jarak < 8)
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
