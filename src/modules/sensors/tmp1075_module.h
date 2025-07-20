#ifndef TMP1075_MODULE_H
#define TMP1075_MODULE_H

#include "module.h"
#include "TMP1075.h"
#include "dynconf.h"

struct Tmp1075Module : public SensorModule {
public:
    Tmp1075Module(const char* module_name, const char* module_config);

    static Tmp1075Module* make_tmp();
private:
    TMP1075* tmp1075 = nullptr;

    bool run_init() override;
    bool run_update() override;
    bool run_calibration() override;
};

#endif /* TMP1075_MODULE_H */