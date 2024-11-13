#include "MIXER.h"
#include "data.h"

sysClass::sysClass() 
  :flightMode(State::INIT),
  initialised(false), separated(false), deployed(false), chuteOpened(false)
{
  time.second = 0;
  time.nanosecond = 0;
  timeTransition.second = 0;
  timeTransition.nanosecond = 0;
}

sysStatus sysClass::get() {
  sysStatus sysOut;
  sysOut.time = time;
  sysOut.timeTransition = timeTransition;
  sysOut.flightMode = flightMode;
  sysOut.initialised = initialised;
  sysOut.separated = separated;
  sysOut.deployed = deployed;
  sysOut.chuteOpened = chuteOpened;
  sysOut.mix = mix.get();
  sysOut.out = out.get();
  return sysOut;
}
void sysClass::setReady() { initialised = true; }
void sysClass::separate() { separated = true; }
void sysClass::deploy()   { deployed = true; }
void sysClass::openChute() { chuteOpened = true; }
void sysClass::setFlightMode(FLIGHTMODE flightModeIn) { flightMode = flightModeIn; }
void sysClass::setTransitionTime(timeCode timeIn) { timeTransition = timeIn; }
void sysClass::setTime(timeCode timeIn) { time = timeIn; }
bool sysClass::isInitialised() { return initialised; }

mixClass::mixClass() 
{
}

mixStatus mixClass::get() {
  mixStatus mixOut;
  mixOut.ventN2O = ventN2O;
  mixOut.ventFuel = ventFuel;
  mixOut.pressurizer = pressurizer;
  mixOut.solenoid4 = solenoid4;
  mixOut.servoN2O = servoN2O;
  mixOut.servoFuel = servoFuel;
  mixOut.ignitor = ignitor;
  mixOut.buzzer = buzzer;
  return mixOut;
}

// Return the command that matches the current flight mode
mixStatus mixClass::compute(sysStatus sysIn) {
  mixStatus mixOut;
  switch (sysIn.flightMode) {
    //TODO : add the other modes
      default:
          break;
  }
  ventN2O = mixOut.ventN2O;
  ventFuel = mixOut.ventFuel;
  solenoid4 = mixOut.solenoid4;
  servoN2O = mixOut.servoN2O;
  servoFuel = mixOut.servoFuel;

  pressurizer = mixOut.pressurizer;

  // Safety feature. If mode wasn't ARMED, we force the ignitor and pressurize off.
  // Otherwise, we force the buzzer to be on.

  if (sysIn.flightMode != FLIGHTMODE::ARMED_MODE) {
    ignitor = IGNITOR_INACTIVE;
    buzzer = mixOut.buzzer;
  } else {
    ignitor = mixOut.ignitor;
    buzzer = BUZZER_ACTIVE;
  }

  return mixOut;
}

// The system is on ground
mixStatus mixClass::mixInit(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_OPEN;
  mixOut.ventFuel     = VENT_FUEL_OPEN;
  mixOut.pressurizer  = PRESSURIZER_CLOSED;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}

mixStatus mixClass::mixReadySteady(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_OPEN;
  mixOut.ventFuel     = VENT_FUEL_OPEN;
  mixOut.pressurizer  = PRESSURIZER_CLOSED;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}

mixStatus mixClass::mixCalibration(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_CLOSED;
  mixOut.ventFuel     = VENT_FUEL_CLOSED;
  mixOut.pressurizer  = PRESSURIZER_CLOSED;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}

mixStatus mixClass::mixManual(sysStatus sysIn) {
  return mixManualMemory;
}

mixStatus mixClass::mixArmed(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_CLOSED;
  mixOut.ventFuel     = VENT_FUEL_CLOSED;
  mixOut.pressurizer  = PRESSURIZER_CLOSED;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}

// The engine is pressured and ready for ignition.
// Can move to IGNITER with the IGNITE command.
mixStatus mixClass::mixPressured(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_CLOSED;
  mixOut.ventFuel     = VENT_FUEL_CLOSED;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = BUZZER_ACTIVE;
  return mixOut;
}

// The igniter is fired.
// After IGNITER_COUNTER is elapsed, we move to IGNITION.
mixStatus mixClass::mixIgniter(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_CLOSED;
  mixOut.ventFuel     = VENT_FUEL_CLOSED;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_ACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}


// Fuel and Oxydizer valves are partially opened.
// After IGNITION_COUNTER is elapsed, we move to THRUST.
mixStatus mixClass::mixIgnition(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_CLOSED;
  mixOut.ventFuel     = VENT_FUEL_CLOSED;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;

  if (timeDiff(sysIn.time, sysIn.timeTransition) < STEP1_DELAY_MS/1000.0) {
    mixOut.servoN2O     = SERVO_N2O_STEP_1;
    mixOut.servoFuel    = SERVO_FUEL_STEP_1;
  } else if (timeDiff(sysIn.time, sysIn.timeTransition) < (STEP2_DELAY_MS+STEP1_DELAY_MS)/1000.0) {
    mixOut.servoN2O     = SERVO_N2O_STEP_2;
    mixOut.servoFuel    = SERVO_FUEL_STEP_1;
  }
  else if (timeDiff(sysIn.time, sysIn.timeTransition) > (STEP2_DELAY_MS+STEP1_DELAY_MS)/1000.0) {
    mixOut.servoN2O     = SERVO_N2O_STEP_2;
    mixOut.servoFuel    = SERVO_FUEL_STEP_2;
  }
  return mixOut;
}


// Fuel and Oxydizer valves are fully opened.
// After THRUST_COUNTER is elapsed, we move to SHUTDOWN.
mixStatus mixClass::mixThrust(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_CLOSED;
  mixOut.ventFuel     = VENT_FUEL_CLOSED;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_OPEN;
  mixOut.servoFuel    = SERVO_FUEL_OPEN;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}

// Fuel Valve is closed.
// After SHUTDOWN_COUNTER is elapsed, we move to ASCENT.
mixStatus mixClass::mixShutdown(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_CLOSED;
  mixOut.ventFuel     = VENT_FUEL_CLOSED;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_OPEN;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}

mixStatus mixClass::mixFlightAscent(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_OPEN;
  mixOut.ventFuel     = VENT_FUEL_OPEN;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = BUZZER_ACTIVE;
  return mixOut;
}

mixStatus mixClass::mixFlightDescent(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_OPEN;
  mixOut.ventFuel     = VENT_FUEL_OPEN;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = !BUZZER_ACTIVE;
  return mixOut;
}

mixStatus mixClass::mixFlightGliding(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_OPEN;
  mixOut.ventFuel     = VENT_FUEL_OPEN;
  mixOut.pressurizer  = PRESSURIZER_OPEN;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = BUZZER_ACTIVE;
  return mixOut;
}

mixStatus mixClass::mixAbort(sysStatus sysIn) {
  mixStatus mixOut({0, 0, 0, 0, 0, 0, 0, 0});
  mixOut.ventN2O      = VENT_N2O_OPEN;
  mixOut.ventFuel     = VENT_FUEL_OPEN;
  mixOut.pressurizer  = PRESSURIZER_CLOSED;
  mixOut.solenoid4    = SOLENOID4_CLOSED;
  mixOut.servoN2O     = SERVO_N2O_CLOSED;
  mixOut.servoFuel    = SERVO_FUEL_CLOSED;
  mixOut.ignitor      = IGNITOR_INACTIVE;
  mixOut.buzzer       = BUZZER_ACTIVE;
  return mixOut;
}

void mixClass::setManualMemory(mixStatus mixIn) {
  mixManualMemory = mixIn;
}




