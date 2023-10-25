/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include "BluetoothTask.h"

BluetoothTask::BluetoothTask(Led *led, ServoMotor *servo, int rxPin, int txPin,
                             Scheduler *Scheduler,
                             const char taskName[MAX_MESSAGE_LENGTH],
                             unsigned long period)
    : Task(Scheduler, taskName, period) {
  this->led1 = led;
  this->servo = servo;
  this->message[0] = '\0';
  this->Bluetooth = new SoftwareSerial(rxPin, txPin);
  Bluetooth->begin(9600);
}

void BluetoothTask::init() {}

void BluetoothTask::tick() {
  if (!this->canRun()) {
    return;
  }
  this->read();
  //this->send();
  this->readQueue();
  this->updateLastExecution();
}

void BluetoothTask::read() {
  if (!listenerBluetooth()) {
    return;
  }

  char *topic = strtok(this->message, ":");
  char *value = strtok(NULL, "\0");
  bool is_valid = false;

  if (strcmp(topic, "set_light") == 0) {
    if (strcmp(value, "on") == 0) {
      led1->switchOn();
      is_valid = true;
    }
    if (strcmp(value, "off") == 0) {
      led1->switchOff();
      is_valid = true;
    }
  } else if (strcmp(topic, "set_roller_blind") == 0) {
    int val = atoi(value);
    servo->setPosition(val);
    is_valid = true;
  }

  if (is_valid) {
    this->pushQueue("bluetooth");
    this->scheduler->getTask(SERIAL_TASK_NAME)->pushQueue("bluetooth");
  }
}

void BluetoothTask::send() {
  sendLightMessage();
  sendRollerBlindMessage();
}

void BluetoothTask::readQueue() {
  while (!this->queue.isEmpty()) {
    Bluetooth->println("source:" + String(this->queue.dequeue()));
    this->send();
  }
}

void BluetoothTask::sendLightMessage() {
  Bluetooth->println(this->light_messages[led1->getState()]);
}

void BluetoothTask::sendRollerBlindMessage() {
  Bluetooth->println("roller_blind:" + String(servo->getPosition()));
}

bool BluetoothTask::listenerBluetooth() {
  unsigned int message_pos = 0;
  this->message[0] = '\0';

  while (Bluetooth->available() > 0) {
    if (message_pos >= MAX_MESSAGE_LENGTH) {
      message_pos = 0;
    }

    char inByte = Bluetooth->read();

    if (inByte == '\n') {
      this->message[message_pos] = '\0';
      return true;
    } else {
      this->message[message_pos] = inByte;
      message_pos++;
    }
  }
  return false;
}