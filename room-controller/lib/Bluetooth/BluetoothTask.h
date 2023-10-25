#ifndef __BLUETOOTHTASK__
#define __BLUETOOTHTASK__

#include <Arduino.h>

#include "Led.h"
#include "ServoMotor.h"
#include "SoftwareSerial.h"
#include "Scheduler.h"
#include "TaskName.h"
#include "Task.h"

#define MAX_MESSAGE_LENGTH 32

class BluetoothTask : public Task {
  private:
    Led* led1;
    ServoMotor* servo;
    SoftwareSerial* Bluetooth;
    char message[MAX_MESSAGE_LENGTH];
    const char* light_messages[2] = { "light:off", "light:on" };

    void read();
    void send();
    void readQueue();
    void sendLightMessage();
    void sendRollerBlindMessage();
    bool listenerBluetooth();

  public:
    BluetoothTask(Led* led, ServoMotor* servo, int rxPin, int txPin, Scheduler* scheduler, const char taskName[MAX_NAME_LENGTH], unsigned long period);
    void init() override;
    void tick() override;
};

#endif