enum class State
{
    IDLE,
    LANDED,
    DESCENT,
    ASCENT,
    CALIBRATION,
    ERRORGROUND,
    ERRORFLIGHT,
    THRUSTSEQUENCE,
    MANUAL,
    ARMED,
    READY
};

// Path: AV-Firehorn-Rpi/include/flightControl/AvState.h
// Compare this snippet from AV-Firehorn-Rpi/src/flightControl/FSM.cpp:
//     // this function allows to get the current state of the FSM
// functions that transition from one state to others possible states
class AvState
{
public:
    // constructor
    AState();
    // destructor
    ~AvState();

    // this function allows to get the current state of the FSM
    State getCurrentState();
    char *AvStateToString(State state) const;
    State AvState::update(AvData data);

private:
    State fromIdle(AvData data);
    State fromLanded();
    State fromDescent(AvData data);
    State fromAscent(AvData data);
    State fromCalibration(AvData data);
    State fromErrorGround(AvData data);
    State fromErrorFlight();
    State fromThrustSequence(AvData data);
    State fromManual(AvData data);
    State fromArmed(AvData data);
    State fromReady(AvData data);
    State currentState;
    State *possibleStates();
    char *telemetry_set[10];
    char *StatetoString(State state);
};
