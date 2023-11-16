#include "AvState.h"

AvState fromManualVent(){
    return AvState::ManualVent;
}

AvState fromArmed(){
    return AvState::Armed;
}

AvState fromCalibration(){
    return AvState::Calibration;
}

AvState fromPressurization(){
    return AvState::Pressurization;
}

AvState fromThrustSequence(){
    return AvState::ThrustSequence;
}

AvState fromIdleGround(){
    return AvState::IdleGround;
}


AvState fromIdleFlight(){
    return AvState::IdleFlight;
}

AvState fromLandedSafe(){
    return AvState::LandedSafe;
}

AvState fromLandedUnsafe(){
    return AvState::LandedUnsafe;
}

AvState fromRecFirstStage(){
    return AvState::RecFirstStage;
}

AvState fromRecSecondStage(){
    return AvState::RecSecondStage;
}

AvState fromEmergencyRec(){
    return AvState::EmergencyRec;
}

AvState fromVentAsk(){
    return AvState::VentAsk;
}

AvState fromRadioControlled(){
    return AvState::RadioControlled;
}

AvState fromErrorFlight(){
    return AvState::ErrorFlight;
}

AvState fromErrorGround(){
    return AvState::ErrorGround;
}

AvState fromErrorArmed(){
    return AvState::ErrorArmed;
}

char* toString(AvState state){
        switch(state){
            case AvState::ManualVent:
                return "ManualVent";
            case AvState::Armed:
                return "Armed";
            case AvState::Calibration:
                return "Calibration";
            case AvState::Pressurization:
                return "Pressurization";
            case AvState::ThrustSequence:   
                return "ThrustSequence";
            case AvState::IdleGround:   
                return "IdleGround";   
            case AvState::IdleFlight:   
                return "IdleFlight";
            case AvState::LandedSafe:
                return "LandedSafe";
            case AvState::LandedUnsafe: 
                return "LandedUnsafe";
            case AvState::RecFirstStage:    
                return "RecFirstStage";
            case AvState::RecSecondStage:   
                return "RecSecondStage";
            case AvState::EmergencyRec:
                return "EmergencyRec";
            case AvState::VentAsk:
                return "VentAsk";
            case AvState::RadioControlled: 
                return "RadioControlled";
            case AvState::ErrorFlight:
                return "ErrorFlight";
            case AvState::ErrorGround:  
                return "ErrorGround";
            case AvState::ErrorArmed:   
                return "ErrorArmed";
        }
    }



