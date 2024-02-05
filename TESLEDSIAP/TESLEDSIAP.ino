#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define BUZ_PIN D1  // D0 - MERAH BUZZER
#define LED_PIN D0  // D0 - BIRU
#define OKE_PIN D2  // D2 - HIJAU
#define SET_BTN D8  // Push BUtton SET

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  pinMode(OKE_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(OKE_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZ_PIN, LOW);
  delay(1000);
  
  digitalWrite(OKE_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZ_PIN, LOW);
  delay(1000);
  
  digitalWrite(OKE_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZ_PIN, HIGH);
  delay(1000);
  
}
