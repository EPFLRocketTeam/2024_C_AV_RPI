#ifndef PR_BOARD_H
#define PR_BOARD_H

#include "h_driver.h"
#include <array>
#include <cstdint>  // For uint8_t

// FIXME: should be deined in intranet_commands.h ?
#define NUM_VALVES 6

class PR_board : public HDriver {
public:
    PR_board();
    ~PR_board() = default;

    void check_policy(const DataDump& dump, const uint32_t delta_ms) override;

    // FIXME: implement the valve logic later on
    // Valve control functions
    // void write_valve(uint8_t valve_id, ValveOpenDegree degree);
    // ValveOpenDegree read_valve(uint8_t valve_id) const;

private:
    // Each element is 4 bits (we store in uint8_t for simplicity, masking the 4 LSB)
    std::array<uint8_t, NUM_VALVES> current_valve_states = {0};

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
