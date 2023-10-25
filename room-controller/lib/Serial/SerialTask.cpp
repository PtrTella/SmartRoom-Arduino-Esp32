/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include "SerialTask.h"

SerialTask::SerialTask(Led *led, ServoMotor *servo, Scheduler *scheduler,
                       const char taskName[MAX_NAME_LENGTH],
                       unsigned long period)
    : Task(scheduler, taskName, period) {
  Serial.begin(9600);
  Serial.println("SerialTask started");
  this->led1 = led;
  this->servo1 = servo;
  this->message[0] = '\0';
}

void SerialTask::init() {}

void SerialTask::tick() {
  if (!this->canRun()) {
    return;
  }
  this->read();
  this->send();
  this->updateLastExecution();
}

void SerialTask::read() {
  if (!listenerSerial()) {
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
      this->led1->switchOff();
      is_valid = true;
    }
  } else if (strcmp(topic, "set_roller_blind") == 0) {
    int pos = atoi(value);
    this->servo1->setPosition(pos);
    is_valid = true;
  }

  if (is_valid) {
    this->pushQueue("serial");
    this->scheduler->getTask(BLUETOOTH_TASK_NAME)->pushQueue("serial");
  }
}

void SerialTask::send() {
  readQueue();
  sendLightMessage();
  sendRollerBlindMessage();
}

void SerialTask::readQueue() {
  while (!this->queue.isEmpty()) {
    Serial.println("source:" + String(this->queue.dequeue()));
  }
}

void SerialTask::sendLightMessage() {
  Serial.println(this->light_messages[led1->getState()]);
}

void SerialTask::sendRollerBlindMessage() {
  Serial.println("roller_blind:" + String(servo1->getPosition()));
}

bool SerialTask::listenerSerial() {
  unsigned int message_pos = 0;
  this->message[0] = '\0';

  while (Serial.available() > 0) {
    if (message_pos >= MAX_MESSAGE_LENGTH) {
      message_pos = 0;
    }

    char inByte = Serial.read();

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
