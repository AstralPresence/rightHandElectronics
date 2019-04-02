/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef CAP1114_h
#define CAP1114_h

#include "Arduino.h"

class Capsense
{
  public:
     Capsense();
     void Init();
     void toggle(int pin);
     void setSensitivity(int factor);
     byte readSensitivity();
     void setDirection(int pin ,int Direction  );
     int isSet(int pin);
     void setInterrupt(int firing,int zerocrossing, int microseconds);
     void unSetInterrupt(void);
     int readCapTouch();
     void setState(int pin, bool state);
     int readState(int pin);
     void writeRegister(byte regAdress, byte value);
     byte readRegister(int reg);
  private:
   // int _firePin;
    
};

#endif
