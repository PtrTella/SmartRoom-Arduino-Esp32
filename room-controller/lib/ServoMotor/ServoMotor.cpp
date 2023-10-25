/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include "ServoMotor.h"

ServoMotor::ServoMotor(int pin) {
    this->pin = pin;
    on();
}

void ServoMotor::on() {
    servo.attach(pin);
}

void ServoMotor::off() {
    servo.detach();
}

void ServoMotor::setPosition(int pos) {
    if (pos > 180) {
        pos = 180;
    } else if (pos < 0) {
        pos = 0;
    }
    pos = map(pos, 0, 100, 0, 180);
    float coef = ((float)MAX_PULSE_WIDTH - (float)MIN_PULSE_WIDTH) / 180.0;
    servo.write(MIN_PULSE_WIDTH + pos * coef);
    delay(15);
}

int ServoMotor::getPosition() {
    return map(servo.read(), 0, 180, 0, 100);
}