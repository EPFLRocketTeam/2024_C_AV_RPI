#include "FsmState/AvState.h"

#include <String>
#include "AvState.h"

class FSM
{
private:
    AvState currentState;
    // this function calls the logical functions that updates the state of the FSM
    AvState update()
    {
        switch (currentState)
        {
        case AvState::ManualVent:
            return fromManualVent();
        case AvState::Armed:
            return fromArmed();
        case AvState::Calibration:
            return fromCalibration();
        case AvState::Pressurization:
            return fromPressurization();
        case AvState::ThrustSequence:
            return fromThrustSequence();
        case AvState::IdleGround:
            return fromIdleGround();
        case AvState::IdleFlight:
            return fromIdleFlight();
        case AvState::LandedSafe:
            return fromLandedSafe();
        case AvState::LandedUnsafe:
            return fromLandedUnsafe();
        case AvState::RecFirstStage:
            return fromRecFirstStage();
        case AvState::RecSecondStage:
            return fromRecSecondStage();
        case AvState::EmergencyRec:
            return fromEmergencyRec();
        case AvState::VentAsk:
            return fromVentAsk();
        case AvState::RadioControlled:
            return fromRadioControlled();
        case AvState::ErrorFlight:
            return fromErrorFlight();
        case AvState::ErrorGround:
            return fromErrorGround();
        case AvState::ErrorArmed:
            return fromErrorArmed();
        }
    }
    

public:
    // constructor
    FSM()
    {
        currentState = AvState::IdleGround;
    }
    // destructor
    ~FSM()
    {
        currentState = AvState::None;
    }
    
    // this function allows to get the current state of the FSM
    AvState getCurrentState()
    {
        return currentState;
    }
    void NextState()
    {
        currentState = update();
    }
    char *FSMtoString() const
    {
        std::string stateStr = AvStatetoString(currentState);
        std::string result = "Current state of rocket is " + stateStr;
        char *cstr = new char[result.length() + 1];
        std::strcpy(cstr, result.c_str());
        return cstr;
    }
};
