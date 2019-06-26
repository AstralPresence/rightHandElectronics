#define relay 13

void setup() {
  // put your setup code here, to run once:

pinMode(relay, OUTPUT);
}

void loop() {

  digitalWrite(relay,HIGH);
  delay(1000);
  digitalWrite(relay,LOW);
  delay(1000);
  // put your main code here, to run repeatedly:

}
