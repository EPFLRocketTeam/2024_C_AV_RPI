#ifndef OUTPUT_H
#define OUTPUT_H

#include "Arduino.h"
#include "CONFIG.h"
#include "PWMServo.h"

// Contains the informations send to the servo motor
struct outStatus {
  bool ventN2O;
  bool ventFuel;
  bool pressurizer;
  bool solenoid4;
  unsigned servoN2O;
  unsigned servoFuel;
  bool ignitor;
  bool buzzer;
};

struct mixStatus {
  bool ventN2O;
  bool ventFuel;
  bool pressurizer;
  bool solenoid4;
  unsigned servoN2O;
  unsigned servoFuel;
  bool ignitor;
  bool buzzer;
};

class outClass {
  public:
    outClass();
    void write();
    outStatus get();
    outStatus compute(mixStatus);
  private:
    PWMServo PWMServoN2O;
    PWMServo PWMServoFuel;
    bool ventN2O;
    bool ventFuel;
    bool pressurizer;
    bool solenoid4;
    unsigned servoN2O;
    unsigned servoFuel;
    bool ignitor;
    bool buzzer;
};


#endif
