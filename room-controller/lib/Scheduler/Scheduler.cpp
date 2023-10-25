/*
 * Project carried out by the following group members:
 * Baroncini  Ugo     ugo.baroncini@studio.unibo.it
 * Bighini    Luca    luca.bighini@studio.unibo.it
 * Tellarini  Pietro  pietro.tellarini2@studio.unibo.it
*/

#include "Scheduler.h"

Scheduler::Scheduler() {
    this->scheduledTasksNumber = 0;
}

bool Scheduler::schedule(Task* t) {
    if (!t or this->scheduledTasksNumber >= MAXN_TASKS) {
        return false;
    }
    this->tasks[this->scheduledTasksNumber] = t;
    this->scheduledTasksNumber++;
    return true;
}

void Scheduler::tick() {
    for (uint8_t i = 0; i < this->scheduledTasksNumber; i++) {
        if (tasks[i]->isEnabled()) {
            tasks[i]->tick();
        }
    }
}


Task* Scheduler::getTask(const char name[MAX_NAME_LENGTH]) {
    for (uint8_t i = 0; i < this->scheduledTasksNumber; i++) {
        if (strcmp(tasks[i]->getName(), name) == 0) {
            return tasks[i];
        }
    }
    return NULL;
}