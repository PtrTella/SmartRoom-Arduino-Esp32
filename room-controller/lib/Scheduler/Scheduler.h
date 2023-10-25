#ifndef __SCHEDULER_H_INCLUDED__
#define __SCHEDULER_H_INCLUDED__

#include <Arduino.h>
#include "Task.h"

#define MAXN_TASKS 8


class Scheduler {
  public:
    Scheduler();
    bool schedule(Task* t);
    void tick();
    Task* getTask(const char name[MAX_NAME_LENGTH]);

  private:
    Task* tasks[MAXN_TASKS];
    uint8_t scheduledTasksNumber;
};

#endif