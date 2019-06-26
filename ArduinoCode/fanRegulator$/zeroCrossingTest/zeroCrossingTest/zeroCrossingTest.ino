#define zeroCrossing 12

void setup() {

  Serial.begin(115200);
pinMode(zeroCrossing , INPUT);
}

void loop() {
  // uncomment this 2 lines to see pulses in serial plotter while commenting next two lines
  int x = digitalRead(zeroCrossing);
  Serial.println(x);

//  double timeOn = pulseIn(zeroCrossing,HIGH);//returns on period of pulse in microseconds 
//Serial.println(timeOn);
}
