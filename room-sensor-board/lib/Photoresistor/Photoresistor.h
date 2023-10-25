#ifndef __PHOTORESISTOR__
#define __PHOTORESISTOR__

class Photoresistor {
 public:
  Photoresistor(int pin);
  int getIntensity();

 private:
  int pin;
};

#endif