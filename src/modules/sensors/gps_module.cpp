
#include "gps_module.h"

bool GPSModule::run_init()
{
    i2c_gps  = new I2CGPS();
    tiny_gps = new TinyGPSPlus();
    if (i2c_gps == NULL || tiny_gps == NULL)
    {
        return false;
    }

    return true;
};
bool GPSModule::run_update()
{
    if (i2c_gps == NULL || tiny_gps == NULL)
    {
        return false;
    }

    while (i2c_gps->available()) {
        tiny_gps->encode(i2c_gps.read());
    }

    if (tiny_gps->time.isUpdated()) {
        if (tiny_gps->date.isValid()) {
            unsigned year  (tiny_gps->date.year());
            unsigned month (tiny_gps->data.month());
            unsigned day   (tiny_gps->data.day());

            Data::get_instance().write(Data::NAV_GNSS_TIME_YEAR,  &year);
            Data::get_instance().write(Data::NAV_GNSS_TIME_MONTH, &month);
            Data::get_instance().write(Data::NAV_GNSS_TIME_DAY,   &day);
        }
        if (tiny_gps->time.isValid()) {
            unsigned hour   (tiny_gps->time.hour());
            unsigned minute (tiny_gps->time.minute());
            unsigned second (tiny_gps->time.second());
            unsigned centi  (tiny_gps->time.centisecond());

            Data::get_instance().write(Data::NAV_GNSS_TIME_HOUR,   &hour);
            Data::get_instance().write(Data::NAV_GNSS_TIME_MINUTE, &minute);
            Data::get_instance().write(Data::NAV_GNSS_TIME_SECOND, &second);
            Data::get_instance().write(Data::NAV_GNSS_TIME_CENTI,  &centi);
        }
        if (tiny_gps->location.isValid()) {
            double latitute  (tiny_gps->location.lat());
            double longitude (tiny_gps->location.lng());
            
            Data::get_instance().write(Data::NAV_GNSS_POS_LAT, &latitute);
            Data::get_instance().write(Data::NAV_GNSS_POS_LNG, &longitude);
        }
        if (tiny_gps->altitude.isValid()) {
            double altitude(tiny_gps->altitude.meters());

            Data::get_instance().write(Data::NAV_GNSS_POS_ALT, &altitude);
        }
        if (tiny_gps->course.isValid()) {
            double course (tiny_gps->course.deg());

            Data::get_instance().write(Data::NAV_GNSS_COURSE, &course);
        }
    }

    return true;
};
bool GPSModule::run_calibration()
{
    return true;
};
GPSModule::GPSModule(
        const char* module_name, 
        const char* module_config
    )
    : SensorModule(
          module_name,
          module_config) {}

GPSModule* GPSModule::make_gps () {
    return new GPSModule(
        "Sensors::GPS",
        "sensors.gps"
    );
}
