#include "FSM1.0/FSM.h"

class Control{
private:
    FSM fsm;
public:
    Control(){
        fsm = FSM();
    }
    ~Control(){
        fsm.~FSM(); 
    }
    void execute(){
        while (true){
            fsm.update();
        }
    }
};




