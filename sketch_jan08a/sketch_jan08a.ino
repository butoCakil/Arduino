#define LED 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("LED Mati");
  digitalWrite(LED, HIGH);
  delay(2000);

  Serial.println("LED Hidup");
  digitalWrite(LED, LOW);
  delay(2000);
}
