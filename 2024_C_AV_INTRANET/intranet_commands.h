//
// Created by marin on 08.03.2025.
//

#ifndef FLIGHT_COMPUTER_INTRANET_COMMANDS_H
#define FLIGHT_COMPUTER_INTRANET_COMMANDS_H

#define VE_NO  (1 << 5)  
#define VO_NOC (1 << 4)
#define ME_B   (1 << 3)
#define MO_BC  (1 << 2)
#define IE_NC  (1 << 1)
#define IO_NCC (1 << 0)

enum class PRBCommand {
    // Write Commands
    TIMESTAMP_MAIN_FSM,
    WAKE_UP,
    CLEAR_TO_IGNITE,
    // Read Commands
    IS_WOKEN_UP,
    FSM_PRB,
    P_OIN,
    T_OIN,
    P_EIN,
    T_EIN,
    P_CCC,
    T_CCC,
    P_CIG,
    T_CIG,
    // Read/Write Command
    STATES_VANNES
};

#endif //FLIGHT_COMPUTER_INTRANET_COMMANDS_H
