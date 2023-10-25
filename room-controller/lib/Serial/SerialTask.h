#ifndef __SERIALTASK__
#define __SERIALTASK__

#include <Arduino.h>
#include "Led.h"
#include "ServoMotor.h"
#include "Task.h"
#include "Scheduler.h"
#include "TaskName.h"

#define MAX_MESSAGE_LENGTH 32

class SerialTask : public Task {
  private:
    Led *led1;
    ServoMotor *servo1;
    char message[MAX_MESSAGE_LENGTH];
    const char* light_messages[2] = { "light:off", "light:on" };

    void read();
    void send();
    void readQueue();
    bool listenerSerial();
    void sendLightMessage();
    void sendRollerBlindMessage();

  public:
    SerialTask(Led *led, ServoMotor *servo, Scheduler* scheduler, const char taskName[MAX_NAME_LENGTH], unsigned long period);
    void init() override;
    void tick() override;
};

#endif