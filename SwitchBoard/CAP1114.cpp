/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "CAP1114.h"
#include "Wire.h"
#include "math.h"
#include "Ticker.h"  //Ticker Library

Ticker dimmer;


#define INPUT 0
#define OUTPUT 1
int _firePin;



bool writeByteData(byte deviceAddress, byte regAddress, byte value) {

  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(value); 
 return Wire.endTransmission();
 
}

void Capsense::writeRegister(byte regAdress, byte value){
  
  
  writeByteData(0x28, regAdress,value);
  
  
  }
byte readByteData(int devAddress, int regAddress) {

  Wire.beginTransmission(devAddress);
  Wire.write(regAddress);
  Wire.endTransmission();

  Wire.requestFrom(devAddress, 1);

  if (Wire.available()) {
    byte value = Wire.read();
    
  
     return value;
    
  }

}


void changeState(){
     
     writeByteData(0x28, 0x73, readByteData(0X28,0X73) |  (byte)pow(2,(_firePin - 1)  ) ); //set one
     delayMicroseconds(10);
     writeByteData(0x28, 0x73, readByteData(0X28,0X73) &  (0xff  ^ (byte)pow( 2, (_firePin -1) )  ) ); //set zero
    // Serial.println(millis());


   

     
   }


 
Capsense::Capsense()
{

  
}




void Capsense::setState(int  pin, bool state)
{
  byte value;
  if(state){
  value  = readByteData(0x28, 0x73) | (byte)pow(2, (pin-1) ) ;  // to set one
     }
    else {
       
   value = readByteData(0x28, 0x73) & (0xff  ^ (byte)pow(2, (pin-1) ) );   // to set zero
    }
 
 writeByteData(0x28, 0x73, value);
 
}

void Capsense::toggle(int pin)
{
   byte value;
   value = readByteData(0x28, 0x73) ^ (byte)pow(2,(pin-1));
   
   writeByteData(0x28, 0x73, value);

  
  }


void Capsense::setSensitivity(int factor){


  byte value;
  value = ((8-factor)<<4) | (0x0F);
  
  if(!writeByteData(0x28, 0x1F, value)){Serial.println("senstivity Set");}
  else 
  {Serial.println("senstivity not Set");}
  
  }
  
byte Capsense::readSensitivity(){


  byte value   =  readByteData(0x28,0x1F);
  
  value = (value & 0xF0) >> 4;
  value = (8-value);
  return value;
   }

int Capsense::readCapTouch(){
  
  byte value = readByteData(0x28, 0x03);
  //Serial.println(value,BIN);
  
  writeByteData(0x28, 0x00, 0x00);
  
  if(value)
  return ( log10(value)/log10(2) ) + 1 ;
  else
  return 0;
     
}

int Capsense::isSet(int pin){


  
 return readByteData(0x28, 0x73) & (1 << (pin-1))?1:0;
  
}


void Capsense::setDirection(int pin, int Direction){

  byte value;
  if(Direction){
  value  = readByteData(0x28, 0x70) | (byte)pow(2, (pin-1) ) ;  // to set one
    
     }
    else {
       
   value = readByteData(0x28, 0x70) & (0xff  ^ (byte)pow(2, (pin-1) ) );   // to set zero
    
    }
 
 writeByteData(0x28, 0x70, value);
 
 
  
  }



int Capsense::readState(int pin){
  
  
  return readByteData(0x28, 0x72) & (1 << (pin-1))?1:0;
  }


void Capsense::setInterrupt(int firing ,int zerocrossing, int microseconds ){
     
   //  Serial.print(microseconds);
     //Serial.println(" : Interrupt set");
    _firePin =  firing;
    this->Capsense::setDirection(zerocrossing, INPUT); 
   // this->Capsense::unSetInterrupt();
    Serial.println("interrupt unset");
     int T_execution = 0;
    int error = 0;
    
     int  actualDelay = microseconds - T_execution + error;
     
   int x   =  this->Capsense::readState(zerocrossing);
    while(1){
        yield();
      if(this->Capsense::readState(zerocrossing)){
        break;
        }
      
      }
     Serial.println(micros());
    delayMicroseconds(actualDelay);
      Serial.println(micros());
     dimmer.attach_ms(10, changeState);
 
   Serial.println("Interrupt set");
   Serial.println(micros());
    
  
  }

  
void Capsense::unSetInterrupt(){
  //Serial.println("");
  dimmer.detach(); 
  }

byte Capsense::readRegister(int reg){
  
  return readByteData(0x28,reg);
  }

void Capsense::Init(){
    Wire.begin();
  writeByteData(0x28, 0x73, 0x00);
  writeByteData(0x28, 0x74, 0x00);
  writeByteData(0x28, 0x40, 0xff);
  writeByteData(0x28, 0x77, 0x00);
  writeByteData(0x28, 0x78, 0x00);
  writeByteData(0x28, 0x80, 0x00);
  writeByteData(0x28, 0x75, 0xff);
  writeByteData(0x28, 0x71, 0xff);
  writeByteData(0x28, 0x70, 0xff);
  writeByteData(0x28, 0x1f, 0x2f);
 if( !writeByteData(0x28, 0x4f, 0x00))
  {Serial.println("Initialization Done");}
  else {Serial.println("Init not done");
  }
  }



