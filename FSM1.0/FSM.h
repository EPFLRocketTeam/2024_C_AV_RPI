#ifndef FSM_H
#define FSM_H

#include "FsmState/AvState.h"

class FSM
{
private:
    AvState currentState;
  

public:
    FSM();
    ~FSM();
   
    AvState update(AvState instate);
    char *toString(AvState instate);
    AvState getCurrentState();

    char *FSMtoString() const;

};
