int u = 2;
int o = A0;
int rl = 9;
int i, j;
volatile int count = 0;
void setup() {
 pinMode(u, INPUT_PULLUP);
 pinMode(o, OUTPUT);
 pinMode(rl, OUTPUT);
 pinMode(13, OUTPUT);
 attachInterrupt(digitalPinToInterrupt(u), count, RISING);
}
void loop() 
{
  count = 0;
    for(j=0;j<65;j++)
      {
      for (i = 0; i < 6; i++)
        {
 
        delay(1);
        

    if(count==10){ ///////embuh
        digitalWrite(rl, HIGH);
        digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(rl, LOW);
        digitalWrite(13, LOW);
        delay(1000);
        }

    else {
        digitalWrite(rl, LOW);
        digitalWrite(13, LOW);
        }
        delay(100); 
}
