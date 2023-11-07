#include <HCSR04.h>

UltraSonicDistanceSensor distanceSensor(4, 3);  // Initialize sensor that uses digital pins 4-trig and 3-echo.

void setup () {
    Serial.begin(9600);  // We initialize serial connection so that we could print values from sensor.
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
}

void loop () {
    int jarak;
    jarak = distanceSensor.measureDistanceCm();
    if(jarak > 7 & jarak < 15)
      {
        digitalWrite(0, HIGH);
        digitalWrite(1, HIGH);
        delay(1000);
      }
     else{digitalWrite(0, LOW); 
          digitalWrite(1, LOW);}
}
