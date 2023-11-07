#include <HCSR04.h>
// Initialize sensor that uses digital pins 4-trig and 3-echo.
UltraSonicDistanceSensor distanceSensor(4, 3);  
const int tombol = 2;
const int selenoid = 0;
const int led = 1;
unsigned long interval=5000;
unsigned long terakhirMillis=0; 
bool statusOn = true; 
bool lastButtonState1 = true;
int statusTombol = 0;
int buttonState1; 
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 

void setup () {
    Serial.begin(9600);  
    pinMode(selenoid, OUTPUT);
    pinMode(led, OUTPUT);
    pinMode(tombol, INPUT);
}

void onOff(){
  int reading1 = digitalRead(tombol);

  if (reading1 != lastButtonState1) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;

      if (buttonState1 == HIGH) {
        statusOn = !statusOn;
      }
    }
  }

  lastButtonState1 = reading1;
}

void loop () {
    onOff();
    int jarak = distanceSensor.measureDistanceCm();
    unsigned long sekarangMillis = millis();

    digitalWrite(led, statusOn);
    
    if ((unsigned long)(sekarangMillis - terakhirMillis) >= interval) 
                                { statusOn = false;}

    if(jarak > 7 & jarak < 14)  { terakhirMillis = millis();
                                  digitalWrite(0, HIGH);
                                  delay(1000);}
    else                        { digitalWrite(0, LOW);}  

    
}
