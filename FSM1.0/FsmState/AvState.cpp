#include "AvState.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//here  must be replaced by an AVDATA variable object 
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

// parameters of this function will take just the object tht contains the actual data, that will inturn
//call the functions "pressurized()" and "noError()" to get the actual values
/*eg:
fromArmed(AvData avdata){
if(pressurized(avdata) && noError(avdata) && avdata.telemMess._Equals("READY")")){
    return AvState::READY;
}
else{
    return AvState::Armed;
}
*/
AvState fromManual()
{
    if (pressurized())
    {
        return AvState::READY;
    }
    else
    {
        return AvState::MANUAL;
    }
}
AvState fromArmed()
{
    if (pressurized())
    {
        return AvState::READY;
    }
    else
    {
        return AvState::ARMED;
    }
}
AvState fromCalibration()
{
    if (pressurized())
    {
        return AvState::READY;
    }
    else
    {
        return AvState::ERRORGROUND;
    }
}
AvState fromThrustSequence()
{
    if (noError())
    {
        if (thrustsequenceEnded)
        {
            return AvState::ASCENT;
        }
        else
        {
            return AvState::THRUSTSEQUENCE;
        }
    }
    else
    {
        return AvState::ERRORGROUND;
    }
}
AvState fromAscent()
{
    if (noError())
    {
        if (apogeeReached)
        {
            return AvState::DESCENT;
        }
        else
        {
            return AvState::ASCENT;
        }
    }
    else
    {
        return AvState::ERRORFLIGHT;
    }
}
AvState fromDescent()
{
    if (noError())
    {
        if (landed)
        {
            return AvState::LANDED;
        }
        else
        {
            return AvState::DESCENT;
        }
    }
    else
    {
        return AvState::ERRORFLIGHT;
    }
}
AvState fromLanded()
{
    if (noError())
    {
        return AvState::IDLE;
    }
    else
    {
        return AvState::ERRORGROUND;
    }
}   

AvState fromIdle()
{
    if (noError())
    {
        return AvState::IDLE;
    }
    else
    {
        return AvState::ERRORGROUND;
    }
}  
AvState fromErrorGround()
{
    if (noError())
    {
        return AvState::IDLE;
    }
    else
    {
        return AvState::ERRORGROUND;
    }
}

AvState fromErrorFlight()
{
    if (noError())
    {
        return AvState::IDLE;
    }
    else
    {
        return AvState::ERRORFLIGHT;
    }
}

AvState fromReady()
{
    if (noError())
    {
        return AvState::READY;
    }
    else
    {
        return AvState::ERRORGROUND;
    }
}



char *AvStatetoString(AvState state)
{
    switch(state){
        case AvState::IDLE:
            return "IDLE";
        case AvState::LANDED:
            return "LANDED";
        case AvState::DESCENT:
            return "DESCENT";
        case AvState::ASCENT:
            return "ASCENT";
        case AvState::CALIBRATION:
            return "CALIBRATION";
        case AvState::ERRORGROUND:
            return "ERRORGROUND";
        case AvState::ERRORFLIGHT:
            return "ERRORFLIGHT";
        case AvState::THRUSTSEQUENCE:
            return "THRUSTSEQUENCE";
        case AvState::ARMED:
            return "ARMED";
        case AvState::READY:
            return "READY";
        case AvState::MANUAL:
            return "MANUAL";
        default:
            return "ERROR";


    }
}
