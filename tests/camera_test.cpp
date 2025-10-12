#include "camera.h"
#include "logger.h"
#include "intranet_commands.h"
#include "data.h"
#include "av_timer.h"

int main() {
    Logger::init();
    Camera camera(AV_NET_ADDR_CAM_SEP);
    
    while (1) {
        auto tmsp(AvTimer::tick());
        Data::get_instance().write(Data::AV_TIMESTAMP, &tmsp);
        auto dump(Data::get_instance().get());
        camera.send_start_recording();
        AvTimer::sleep(1000);
    }
    Logger::terminate();
}
