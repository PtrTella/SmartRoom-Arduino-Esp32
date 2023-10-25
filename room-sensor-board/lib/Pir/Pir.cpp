/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include <Arduino.h>
#include "Pir.h"

#define CALIBRATION_TIME_SEC 5

Pir::Pir(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT);

  Serial.begin(115200);
  Serial.print("Calibrating PIR sensor... ");

  for (int i = 0; i < CALIBRATION_TIME_SEC; i++) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("PIR SENSOR READY.");
  delay(50);
}

bool Pir::detectedMotion() { return digitalRead(pin); }
