#ifndef PR_BOARD_H
#define PR_BOARD_H

#include "h_driver.h"
#include <cstdint>  // For uint8_t

class PR_board : public HDriver {
public:
    PR_board();
    ~PR_board() = default;

    void check_policy(Data::GoatReg reg, const DataDump& dump);

    // Valve control functions
    void write_valves(uint8_t valve_states);
    uint8_t read_valves() const;

private:
    uint8_t current_valve_state = 0;  // Store the current valve states

    void handleErrorFlight(const DataDump& dump);
    void handleErrorGround(const DataDump& dump);
    void handleDescent(const DataDump& dump);
    void processManualMode(const DataDump& dump);
    void processReadyState(const DataDump& dump);

    void triggerIgnition();
    void executeAbort();
    void deployRecoverySystem();
};

#endif /* PR_BOARD_H */
