/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-joystick
 */

#define VRX_PIN A0  // Arduino pin connected to VRX pin
#define VRY_PIN A1  // Arduino pin connected to VRY pin
#define BTN 2  // Arduino pin connected to VRY pin

#define LEFT_THRESHOLD 120
#define RIGHT_THRESHOLD 10
#define UP_THRESHOLD 10
#define DOWN_THRESHOLD 120

int xValue = 0;  // To store value of the X axis
int yValue = 0;  // To store value of the Y axis

void setup() {
  Serial.begin(9600);
  pinMode(BTN, INPUT_PULLUP);
}

void loop() {
  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  if (xValue < UP_THRESHOLD) {
    Serial.print("UP");
  }

  if(yValue < RIGHT_THRESHOLD){
    Serial.print("RIGHT");
  }

  if(xValue > DOWN_THRESHOLD){
    Serial.print("DOWN");
  }

  if(yValue > LEFT_THRESHOLD){
    Serial.print("LEFT");
  }

  Serial.print(", x = ");
  Serial.print(xValue);
  Serial.print(", y = ");
  Serial.println(yValue);
}
