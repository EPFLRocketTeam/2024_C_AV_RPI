#include "FsmState/AvState.h"

#include <String>
#include "AvState.h"

class FSM
{
private:
    AvState currentState;
    
    // this function calls the logical functions that updates the state of the FSM
    
    

public:
    // constructor
    FSM()
    {
        currentState = AvState::IDLE;
    }
    // destructor
    ~FSM()
    {
        currentState = AvState::IDLE;
    }
    
    // this function allows to get the current state of the FSM
    AvState getCurrentState()
    {
        return currentState;
    }
    char *FSMtoString() const
    {
        std::string stateStr = AvStatetoString(currentState);
        std::string result = "Current state of rocket is " + stateStr;
        char *cstr = new char[result.length() + 1];
        std::strcpy(cstr, result.c_str());
        return cstr;
    }

    void update()
    {
        switch (currentState){
            case AvState::IDLE:
                currentState = fromIdle();
                break;
            case AvState::LANDED:
                currentState = fromLanded();
                break;
            case AvState::DESCENT:
                currentState = fromDescent();
                break;
            case AvState::ASCENT:
                currentState = fromAscent();
                break;
            case AvState::CALIBRATION:
                currentState = fromCalibration();
                break;
            case AvState::ERRORGROUND: 
                currentState = fromErrorGround();
                break;
            case AvState::ERRORFLIGHT:
                currentState = fromErrorFlight();
                break;
            case AvState::THRUSTSEQUENCE:
                currentState = fromThrustSequence();
                break;
            case AvState::ARMED:
                currentState = fromArmed();
                break;
            case AvState::READY:
                currentState = fromReady();
                break;
            case AvState::MANUAL:
                currentState = fromManual();
                break;
            default:
                currentState = AvState::ERRORGROUND;
                break;
            
        }
        
    }
};
