#ifndef AVSTATE_H
#define AVSTATE_H

enum class AvState
{
    None,
    ManualVent,
    Armed,
    Calibration,
    Pressurization,
    ThrustSequence,
    IdleGround,
    IdleFlight,
    LandedSafe,
    LandedUnsafe,
    RecFirstStage,
    RecSecondStage,
    EmergencyRec,
    VentAsk,
    RadioControlled,
    ErrorFlight,
    ErrorGround,
    ErrorArmed,
};

// Update: AvState
AvState fromManualVent();

AvState fromArmed();

AvState fromCalibration();

AvState fromPressurization();

AvState fromThrustSequence();

AvState fromIdleGround();

AvState fromIdleFlight();

AvState fromLandedSafe();

AvState fromLandedUnsafe();

AvState fromRecFirstStage();

AvState fromRecSecondStage();

AvState fromEmergencyRec();

AvState fromVentAsk();

AvState fromRadioControlled();

AvState fromErrorFlight();

AvState fromErrorGround();

AvState fromErrorArmed();

char* AvStatetoString(AvState state) const;

#endif // AVSTATE_H
