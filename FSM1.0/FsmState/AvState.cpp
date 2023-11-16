#include "AvState.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


float thresholdPressure = 100.0;
float pressure = 0.0;
std::string radMess = "radio message";

bool pressurized()
{
    return (pressure >= thresholdPressure);
}
bool noPressure(){
    return (pressure < thresholdPressure);
}
bool sensorError(){
    return false;
}
bool softwareError(){
    return false;
}
bool thrussequenceError(){
    return false;
}
bool thrustsequenceEnded = false;
bool thrustsequenceStarted = false;
bool recFirstStageEnded = false;
bool recSecondStageEnded = false;
bool apogeeReached =  false;
bool landed = false;
bool noError(){
    return !sensorError() && !softwareError() && !thrussequenceError();
}


AvState fromManualVent()
{
    return AvState::ManualVent;
}

AvState fromArmed()
{
    if (noPressure())
    {
        return AvState::IdleGround;
    }
   
}

AvState fromCalibration()
{
    return AvState::IdleGround;
}

AvState fromPressurization()
{
    return AvState::Armed;
}

AvState fromThrustSequence()
{ 
    if(radMess._Equal("unsafe")){
        return AvState::EmergencyRec;
    }

    return AvState::IdleFlight;
}

AvState fromIdleGround()
{
    if(softwareError()||sensorError()){
        return AvState::ErrorGround;
    }else if(radMess._Equal("calibrate")){
        return AvState::Calibration;
    }else if(radMess._Equal("arm")){
        return AvState::Armed;}
    else{
        return AvState::IdleGround;
    }
    
   
}

AvState fromIdleFlight()
{
    if(radMess._Equal("unsafe")){
        return AvState::LandedSafe;
}else if(landed){
    return AvState::LandedSafe;
}else if (apogeeReached){
    return AvState::RecFirstStage; /* code */
}else if(recSecondStageEnded){
    return AvState::LandedSafe;
    }else{
        return AvState::IdleFlight;
    }}



AvState fromLandedSafe()
{
    return AvState::LandedSafe;
}

AvState fromLandedUnsafe()
{
    return AvState::LandedUnsafe;
}

AvState fromRecFirstStage()
{
    if(recFirstStageEnded){
        return AvState::IdleFlight;
    }else{
    return AvState::RecFirstStage;}
}

AvState fromRecSecondStage()
{
    if(recSecondStageEnded){
        return AvState::IdleFlight;
    }else{
    return AvState::RecSecondtStage;}
}

AvState fromEmergencyRec()
{   if(landed){
        return AvState::LandedUnsafe;
    }
    return AvState::EmergencyRec;
}

AvState fromVentAsk()
{
    return AvState::VentAsk;
}

AvState fromRadioControlled()
{
    return AvState::RadioControlled;
}

AvState fromErrorFlight()
{   
    return AvState::ErrorFlight;
}

AvState fromErrorGround()
{
    return AvState::ErrorGround;
}

AvState fromErrorArmed()
{
    return AvState::ErrorArmed;
}

char *AvStatetoString(AvState state)
{
    switch (state)
    {
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
