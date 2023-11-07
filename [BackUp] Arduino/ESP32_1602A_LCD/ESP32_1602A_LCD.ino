// include the library code:
#include <LiquidCrystal.h>
 
// initialize the library with the numbers of the interface pins
//LiquidCrystal My_LCD(19, 23, 18, 17, 16, 15);
LiquidCrystal My_LCD(13, 12, 14, 27, 26, 25);
 
void setup() {
  // set up the LCD's number of columns and rows:
  My_LCD.begin(16, 2);
  // Print a message to the LCD.
  My_LCD.print("circuitschools.");
}
 
void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  My_LCD.setCursor(0, 1);
  // print the number of seconds since reset:
  My_LCD.print(millis() / 1000);
}
