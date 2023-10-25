/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include "Task.h"

Task::Task(Scheduler* scheduler, const char name[MAX_MESSAGE_LENGTH], unsigned long period) {
    strcpy(this->name, name);
    this->period = period;
    this->scheduler = scheduler;
    this->is_enabled = true;
    this->last_execution = 0;
    this->queue = ArduinoQueue<String>(MAX_QUEUE_LENGTH);
}

void Task::setPeriod(unsigned long period) {
    this->period = period;
}

bool Task::canRun() {
    unsigned long now = millis();
    return is_enabled && (now - last_execution) >= period;
}

void Task::updateLastExecution() {
    last_execution = millis();
}

void Task::enable() {
    is_enabled = true;
}

void Task::disable() {
    is_enabled = false;
}

bool Task::isEnabled() {
    return is_enabled;
}

const char* Task::getName() {
    return name;
}

void Task::pushQueue(String data) {
    this->queue.enqueue(data);
}

void Task::init() {
    // to be overridden
    return;
}

void Task::tick() {
    // to be overridden
    return;
}