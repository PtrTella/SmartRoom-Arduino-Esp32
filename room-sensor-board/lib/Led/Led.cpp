/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include <Arduino.h>
#include "Led.h"

#define BlinkTime 1000

Led::Led(int pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void Led::switchOn() { digitalWrite(pin, HIGH); }

void Led::switchOff() { digitalWrite(pin, LOW); }