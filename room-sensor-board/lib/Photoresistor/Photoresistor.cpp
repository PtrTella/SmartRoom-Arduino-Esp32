/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include <Arduino.h>
#include "Photoresistor.h"

Photoresistor::Photoresistor(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT);
}

int Photoresistor::getIntensity() { return analogRead(pin); }