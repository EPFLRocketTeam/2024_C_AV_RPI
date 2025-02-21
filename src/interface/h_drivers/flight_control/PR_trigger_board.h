#ifndef PR_TRIGGER_BOARD_H
#define PR_TRIGGER_BOARD_H

#include "h_driver.h"

class PR_trigger_board : public HDriver {
public:
    PR_trigger_board();
    ~PR_trigger_board() = default;

    static void check_policy(Data::GoatReg reg, DataDump dump);
};

#endif /* PR_TRIGGER_BOARD_H */
