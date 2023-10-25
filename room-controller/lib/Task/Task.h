#ifndef __TASK_H_INCLUDED__
#define __TASK_H_INCLUDED__

#include <Arduino.h>
#include <ArduinoQueue.h>

#define MAX_MESSAGE_LENGTH 32
#define MAX_NAME_LENGTH 32
#define MAX_QUEUE_LENGTH 8

class Scheduler;

class Task {
  public:
    virtual void init();
    virtual void tick();
    
    Task(Scheduler* scheduler, const char name[MAX_NAME_LENGTH], unsigned long period);
    void pushQueue(String);
    bool isEnabled();
    void enable();
    void disable();
    bool canRun();
    void setPeriod(unsigned long period);
    void updateLastExecution();
    const char* getName();
    Scheduler* scheduler;
    
  protected:
    ArduinoQueue<String> queue;

  private:
    char name[MAX_NAME_LENGTH];
    bool is_enabled = true;
    unsigned long last_execution;
    unsigned long period;
};

#endif