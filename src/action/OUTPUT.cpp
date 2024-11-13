# include "OUTPUT.h"

outClass::outClass() 
{
  PWMServoN2O.attach(SERVO_N2O_PIN, SERVO_N2O_CLOSED, SERVO_N2O_OPEN);
  PWMServoFuel.attach(SERVO_FUEL_PIN, SERVO_FUEL_CLOSED, SERVO_FUEL_OPEN);
  
  pinMode(VENT_N2O_PIN, OUTPUT);
  pinMode(VENT_FUEL_PIN, OUTPUT);
  pinMode(PRESSURIZER_PIN, OUTPUT);
  // pinMode(SOLENOID_4_PIN, OUTPUT);
  pinMode(IGNITOR_PIN, OUTPUT);
  // pinMode(BUZZER_PIN, OUTPUT);

  ventN2O = false;
  ventFuel = false;
  pressurizer = false;
  solenoid4 = false;
  servoN2O = SERVO_N2O_CLOSED;
  servoFuel = SERVO_FUEL_CLOSED;
  ignitor = IGNITOR_INACTIVE;
}

// Transform the data in a way the servo motor can compute them
outStatus outClass::compute(mixStatus mixIn) {
  // This function might seem useless, it's a rest from the R2HOME code structure where
  // computation was required between the mix result (values between -100 and 100) and servo values
  outStatus outOut;
  ventN2O = mixIn.ventN2O;
  ventFuel = mixIn.ventFuel;
  pressurizer = mixIn.pressurizer;
  solenoid4 = mixIn.solenoid4;
  servoN2O = mixIn.servoN2O;
  servoFuel = mixIn.servoFuel;
  ignitor = mixIn.ignitor;
  buzzer = mixIn.buzzer;
  return outOut;
}


// Send the informations to the outputs
void outClass::write() {
  PWMServoN2O.write(map(servoN2O, SERVO_N2O_CLOSED, SERVO_N2O_OPEN, 0, 180));
  PWMServoFuel.write(map(servoFuel, SERVO_FUEL_CLOSED, SERVO_FUEL_OPEN, 0, 180));
  // Serial.println(pressurizer);


  digitalWrite(VENT_N2O_PIN, ventN2O);
  digitalWrite(VENT_FUEL_PIN, ventFuel);
  digitalWrite(PRESSURIZER_PIN, pressurizer);
  // digitalWrite(SOLENOID_4_PIN, solenoid4);
  digitalWrite(IGNITOR_PIN, ignitor);
  // digitalWrite(BUZZER_PIN, buzzer);
}

outStatus outClass::get() {
  outStatus outOut;
  outOut.ventN2O = ventN2O;
  outOut.ventFuel = ventFuel;
  outOut.pressurizer = pressurizer;
  outOut.solenoid4 = solenoid4;
  outOut.servoN2O = servoN2O;
  outOut.servoFuel = servoFuel;
  outOut.ignitor = ignitor;
  outOut.buzzer = buzzer;
  return outOut;
}
