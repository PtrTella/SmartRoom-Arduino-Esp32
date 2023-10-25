/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include <Arduino.h>

#include "BluetoothTask.h"
#include "Led.h"
#include "SerialTask.h"
#include "ServoMotor.h"
#include "SoftwareSerial.h"
#include "Scheduler.h"
#include "TaskName.h"

#define RX_PIN 2  // to be connected to TX of the BT module
#define TX_PIN 3  // to be connected to RX of the BT module
#define LED_PIN 13
#define SERVO_PIN 5

Led* led;
ServoMotor* servoMotor;
SerialTask* serialTask;
BluetoothTask* bluetoothTask;
Scheduler* scheduler;

// void send(const char* source);

void setup() {
    scheduler = new Scheduler(); 
    led = new Led(LED_PIN);
    servoMotor = new ServoMotor(SERVO_PIN);

    led->switchOff();
    servoMotor->setPosition(0);

    serialTask = new SerialTask(led, servoMotor, scheduler, SERIAL_TASK_NAME, 200) ;
    scheduler->schedule(serialTask);
    bluetoothTask = new BluetoothTask(led, servoMotor, RX_PIN, TX_PIN, scheduler, BLUETOOTH_TASK_NAME, 400);
    scheduler->schedule(bluetoothTask);
}

void loop() {
    scheduler->tick();
}