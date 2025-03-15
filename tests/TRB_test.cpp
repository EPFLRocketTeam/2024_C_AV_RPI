#include <iostream>
#include "data.h"
#include "trigger_board.h"

int main() {
    TriggerBoard trb;

    DataDump goat;
    goat.av_state = State::INIT;

    trb.check_policy(goat);

    try {
        trb.send_wake_up();
    }catch(TriggerBoardException& e) {
        std::cout << e.what() << "\n";
    }
}
