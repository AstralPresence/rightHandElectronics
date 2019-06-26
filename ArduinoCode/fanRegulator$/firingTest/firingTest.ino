
#define firing 4



void setup() { 
  pinMode(firing,OUTPUT);
}

void loop() {
  
  digitalWrite(firing , HIGH);
  delay(1000);
  digitalWrite(firing, LOW);
 delay(1000);

}
