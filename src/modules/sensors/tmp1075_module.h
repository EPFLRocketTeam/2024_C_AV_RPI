
#include "module.h"
#include "TMP1075.h"
#include "dynconf.h"

struct Tmp1075Module : public SensorModule {
private:
    TMP1075* tmp1075 = NULL;

    bool run_init () override;
    bool run_update () override;
    bool run_calibration () override;
public:
    Tmp1075Module (
        const char* module_name, 
        const char* module_config
    );

    static Tmp1075Module* make_tmp ();
};
