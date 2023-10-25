/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include "Led.h"

#include <Arduino.h>

Led::Led(int pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);
}

void Led::switchOn() {
    digitalWrite(this->pin, HIGH);
}

void Led::switchOff() {
    digitalWrite(this->pin, LOW);
}

int Led::getState() {
    return digitalRead(this->pin);
}
