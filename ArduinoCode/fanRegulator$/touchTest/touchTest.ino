/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#define led1 15
#define led2 2
#define led3 3
#define led4 1
#define plus 5
#define minus 14
int count = 0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
 pinMode(plus,INPUT);
 pinMode(led1,OUTPUT);
 pinMode(led2,OUTPUT);
 pinMode(led3,OUTPUT);
 pinMode(led4,OUTPUT);
 pinMode(minus,INPUT);
 setArray(0);


//  attachInterrupt(plus, increment, RISING);
//   attachInterrupt(minus, decrement, RISING)
//Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  



int x = digitalRead(plus);
int y = digitalRead(minus);



if(x){
   if(count<8){
  count = count + 1;
  setArray(count);
   }
  }else if(y){
    if(count>0){
    count = count-1;
    setArray(count);
    }
    
    }


delay(100);
}

void setArray(int count){
  
  if(count==0){
    analogWrite(led1,0);
    analogWrite(led2,0);
    analogWrite(led3,1024);
    analogWrite(led4,1024);
    
    }
  else if(count ==1){
     analogWrite(led1,150);
    analogWrite(led2,0);
    analogWrite(led3,1024);
    analogWrite(led4,1024);
    }
  else if(count ==2){
     analogWrite(led1,1024);
    analogWrite(led2,0);
    analogWrite(led3,1024);
    analogWrite(led4,1024);
    
    }

  else if(count ==3){
     analogWrite(led1,1024);
    analogWrite(led2,150);
    analogWrite(led3,1024);
    analogWrite(led4,1024);
     
   } else if (count==4){
    
     analogWrite(led1,1024);
    analogWrite(led2,1024);
    analogWrite(led3,1024);
    analogWrite(led4,1024);
    }

 else if (count==5){
     analogWrite(led1,1024);
    analogWrite(led2,1024);
    analogWrite(led3,876);
    analogWrite(led4,1024);
    }
     else if (count==6){
     analogWrite(led1,1024);
    analogWrite(led2,1024);
    analogWrite(led3,0);
    analogWrite(led4,1024);
    }

     else if (count==7){
     analogWrite(led1,1024);
    analogWrite(led2,1024);
    analogWrite(led3,0);
    analogWrite(led4,876);
    }

     else if (count==8){
    analogWrite(led1,1024);
    analogWrite(led2,1024);
    analogWrite(led3,0);
    analogWrite(led4,0);
    
    }
    
}


void increment(){

   if(count<8){
  count = count + 1;
  setArray(count);
   }
  }

  void decrement(){
     if(count>0){
    count = count-1;
    setArray(count);
    }
    
    }
