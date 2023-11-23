#ifndef AVSTATE_H
#define AVSTATE_H

enum class AvState
{
    IDLE,
    LANDED,
    DESCENT,
    ASCENT,
    CALIBRATION,
    ERRORGROUND,
    ERRORFLIGHT,
    THRUSTSEQUENCE,
    ARMED,
    READY,
    MANUAL,
};


private:
    bool noPressure();
    bool sensorError();
    bool softwareError();
bool pressurized();
public:

AvState fromManual();
AvState fromArmed();
AvState fromCalibration();
AvState fromThrustSequence();
AvState fromAscent();
AvState fromDescent();
AvState fromLanded();
AvState fromIdle();
AvState fromErrorGround();
AvState fromErrorFlight();
AvState fromReady();


char* AvStatetoString(AvState state) const;

#endif // AVSTATE_H
