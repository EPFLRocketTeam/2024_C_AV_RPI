#include "../../include/flightControl/AvState.h"
#include "../../include/flightControl/AvData.h"
#include <string.h>
#include <list>



    // constructor
     State currentState;

    AvState::AvState()
    {
        currentState = State::IDLE;
    }
    // destructor
    AvState::~AvState()
    {
        currentState = State::IDLE;
    }
    
    // this function allows to get the current state of the FSM
    State AvState::getCurrentState()
    {
        return currentState;
    }
    
    std::string telemetry_set[] = {"calibrate", "check", "ready", "launch", "abort", "arme"};
    void sendError(){
        //print in console the "error"
        std::cout << "Error" << std::endl;
    }
    bool error(){
       
        return false;
    }
    
    State fromIdle(AvData data)
    {
        if ((data.telemetry == telemetry_set[0]   ))
        {
            return State::CALIBRATION;
        }
        {
            return State::CALIBRATION;
        }
        return State::IDLE;
    }
    State fromLanded() { return State::LANDED; }
    State fromDescent(AvData data)
    {
        if (data.velocity < 0)
        {
            return State::LANDED;
        }
        return State::DESCENT;
    }
    State fromAscent(AvData data)
    {
        if (data.velocity == 0)
        {
            return State::DESCENT;
        }
        return State::ASCENT;
    }

    State fromCalibration(AvData data)
    {
        if ((data.telemetry== telemetry_set[2]))
        {
            return State::READY;
            // TODO: check errors
        }
        else if (data.pressure > 0.5)
        {
            return State::ARMED;
        }

        return State::CALIBRATION;
    }
    State fromErrorGround(AvData data)
    {
        if (!error() && (data.telemetry== "check"))
        {
            return State::IDLE;
        }
        else
        {
            return State::ERRORGROUND;
        }
    }
    State fromErrorFlight()
    {
        return State::ERRORFLIGHT;
    }
    State fromThrustSequence(AvData data)
    {
        if ((data.telemetry== telemetry_set[4]))
        {
            return State::ERRORFLIGHT;
        }
        else if (data.ignited)
        {
            return State::ASCENT;
        }
        return State::THRUSTSEQUENCE;
    }
    State fromManual(AvData data)
    {
        if (error())
        {
            return State::ERRORGROUND;
        }
        else if ((data.telemetry== telemetry_set[4]))
        {
            return State::ERRORGROUND;
        }
        else if ((data.telemetry== telemetry_set[5]))
        {
            return State::READY;
        }
        return State::MANUAL;
    }
    State fromArmed(AvData data)
    {
        if (error())
        {
            return State::ERRORGROUND;
        }
        else if ((data.telemetry== telemetry_set[4]))
        {
            return State::ERRORGROUND;
        }
        else if ((data.telemetry== telemetry_set[5]))
        {
            return State::READY;
        }
        else if (data.pressure > 0.5)
        {
            return State::ARMED;
        }
        else if (data.telemetry == telemetry_set[3])
        {
            return State::THRUSTSEQUENCE;
        }
        return State::ARMED;
    }
    State fromReady(AvData data)
    {
        if (error())
        {
            return State::ERRORGROUND;
        }
        else if ((data.telemetry== telemetry_set[4]))
        {
            return State::ERRORGROUND;
        }
        else if ((data.telemetry== telemetry_set[5]))
        {
            return State::ARMED;
        }
        return State::READY;
    }
    State *possibleStates();
    
 
   
    bool pressurized(AvData data)
    {
        return data.pressure > 0.5;
    }

    void AvState::update(AvData data)
    {
        switch (currentState)
        {
        case State::IDLE:
            currentState = fromIdle(data);
            break;
        case State::LANDED:
            currentState = fromLanded();

        case State::DESCENT:
            currentState = fromDescent(data);
            break;
        case State::ASCENT:
            currentState = fromAscent(data);
            break;
        case State::CALIBRATION:
            currentState = fromCalibration(data);
            break;
        case State::ERRORGROUND:
            currentState = fromErrorGround(data);
            break;
        case State::ERRORFLIGHT:
            currentState = fromErrorFlight();
            break;
        case State::THRUSTSEQUENCE:
            currentState = fromThrustSequence(data);
            break;
        case State::MANUAL:
            currentState = fromManual(data);
            break;
        case State::ARMED:
            currentState = fromArmed(data);
            break;
        case State::READY:
            currentState = fromReady(data);
            break;
        default:
            currentState = State::ERRORFLIGHT;
        }
    }

    std::string AvState::stateToString(State state)
    {
        switch (state)
        {
        case State::IDLE:
            return "IDLE";
            break;
        case State::LANDED:

            return "LANDED";
            break;
        case State::DESCENT:
            return "DESCENT";
            break;
        case State::ASCENT:
            return "ASCENT";
            break;
        case State::CALIBRATION:
            return "CALIBRATION";
            break;
        case State::ERRORGROUND:
            return "ERRORGROUND";
            break;
        case State::ERRORFLIGHT:
            return "ERRORFLIGHT";
            break;
        case State::THRUSTSEQUENCE:
            return "THRUSTSEQUENCE";
            break;
        case State::MANUAL:
            return "MANUAL";
            break;
        case State::ARMED:
            return "ARMED";
            break;
        case State::READY:
            return "READY";
            break;
        default:
            return "ERROR";
            break;
        }
    }


    