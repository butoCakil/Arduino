// include the library code:
#include <ESP8266WiFi.h>
//#include <Wire.h>
#include "LiquidCrystal.h"

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//const int RS = D2, EN = D3, d4 = D5, d5 = D6, d6 = D7, d7 = D8;

//if you use the NodeMCU 12E the suggested pins are
const int RS = 16, EN = 1, d4 = 2, d5 = 0, d6 = 4, d7 = 5;

LiquidCrystal lcd(RS, EN, d4, d5, d6, d7);

void setup() {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    // Print a message to the LCD.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("hello, world!");
}

void loop() {
// set the cursor to column 0, line 1
// (note: line 1 is the second row, since counting begins with 0):
lcd.setCursor(0, 1);
// print the number of seconds since reset:
lcd.print(millis() / 1000);
}
