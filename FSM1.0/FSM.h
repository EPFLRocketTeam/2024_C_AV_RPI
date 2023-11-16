#ifndef FSM_H
#define FSM_H

#include "FsmState/AvState.h"

class FSM
{
private:
    AvState currentState;
    AvState update();
  

public:
    FSM();
    ~FSM();
   
    void nextState();
    char *toString(AvState instate);
    AvState getCurrentState();

    char *FSMtoString() const;

};
