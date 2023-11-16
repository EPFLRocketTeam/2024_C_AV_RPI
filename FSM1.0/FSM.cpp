#include "FsmState/AvState.h"
#include <String>

class FSM
{
private:
    AvState currentState;
    AvState possibleState[];

public:
    FSM(AvState state){
        currentState = state;
    }

    AvState getCurrentState(){
        return currentState;
    }

    

AvState update(AvState instate){
    switch(instate){
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
};

