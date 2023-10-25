#ifndef __SERVO__
#define __SERVO__

#include <Arduino.h>
#include <Servo.h>
// #include "ServoTimer2.h"

class ServoMotor {
  private:
    int pin;
    Servo servo;

  public:
    ServoMotor(int pin);
    void on();
    void off();
    void setPosition(int pos);
    int getPosition();
};

#endif