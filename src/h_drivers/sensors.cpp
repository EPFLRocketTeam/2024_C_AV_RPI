#include "sensors.h"
#include "INA228.h"
#include "TMP1075.h"
#include "kalman_params.h"
#include "config.h"
#include "data.h"

Sensors::Sensors()
try
    : adxl1(ADXL375_ADDR_I2C_PRIM),
      adxl2(ADXL375_ADDR_I2C_SEC),
      bmi1(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY),
      bmi2(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY),
      bmp1(BMP3_ADDR_I2C_PRIM),
      bmp2(BMP3_ADDR_I2C_SEC),
      i2cgps(),
      gps(),
      tmp1075(TMP1075_ADDR_I2C),
      ina_lpb(INA228_ADDRESS_LPB, INA228_LPB_SHUNT, INA228_LPB_MAX_CUR),
      ina_hpb(INA228_ADDRESS_HPB, INA228_HPB_SHUNT, INA228_HPB_MAX_CUR),
      kalman(INITIAL_COV_GYR_BIAS,
             INITIAL_COV_ACCEL_BIAS,
             INITIAL_COV_ORIENTATION,
             GYRO_COV,
             GYRO_BIAS_COV,
             ACCEL_COV,
             ACCEL_BIAS_COV,
             GPS_OBS_COV,
             ALT_OBS_COV)
{
    tdb = TDB::from_csv("../tests/NavigationSensors.csv");
    if (tdb != nullptr)
    {
        simulation_mode = true;
        tdb->init();

        adxl1_x = tdb->get_time_series("adxl1_x");
        adxl1_y = tdb->get_time_series("adxl1_y");
        adxl1_z = tdb->get_time_series("adxl1_z");

        adxl2_x = tdb->get_time_series("adxl2_x");
        adxl2_y = tdb->get_time_series("adxl2_y");
        adxl2_z = tdb->get_time_series("adxl2_z");

        bmi1_acc_x = tdb->get_time_series("bmi1_acc_x");
        bmi1_acc_y = tdb->get_time_series("bmi1_acc_y");
        bmi1_acc_z = tdb->get_time_series("bmi1_acc_z");

        bmi1_gyro_x = tdb->get_time_series("bmi1_gyro_x");
        bmi1_gyro_y = tdb->get_time_series("bmi1_gyro_y");
        bmi1_gyro_z = tdb->get_time_series("bmi1_gyro_z");

        bmi2_acc_x = tdb->get_time_series("bmi2_acc_x");
        bmi2_acc_y = tdb->get_time_series("bmi2_acc_y");
        bmi2_acc_z = tdb->get_time_series("bmi2_acc_z");

        bmi2_gyro_x = tdb->get_time_series("bmi2_gyro_x");
        bmi2_gyro_y = tdb->get_time_series("bmi2_gyro_y");
        bmi2_gyro_z = tdb->get_time_series("bmi2_gyro_z");

        bmp1_p = tdb->get_time_series("bmp1_p");
        bmp1_t = tdb->get_time_series("bmp1_t");

        bmp2_p = tdb->get_time_series("bmp2_p");
        bmp2_t = tdb->get_time_series("bmp2_t");

        std::cout << "start\n";
    }
    update_status();
}
catch (...)
{
    std::cout << "Sensors init error\n";
}

Sensors::~Sensors() {}

void Sensors::check_policy(const DataDump &dump, const uint32_t delta_ms)
{
    switch (dump.av_state)
    {
    case State::INIT:
    case State::MANUAL:
    case State::ARMED:
        // TODO: low polling rate
        // update();
        break;
    case State::CALIBRATION:
        // TODO: calibration + low polling rate
        // update();
        if (!dump.event.calibrated)
        {
            calibrate();
        }
        break;
    case State::READY:
    case State::THRUSTSEQUENCE:
    case State::LIFTOFF:
    case State::ASCENT:
    case State::DESCENT:
    case State::LANDED:
        // TODO: high polling rate
        update();
        break;
    case State::ERRORGROUND:
        break;
    case State::ERRORFLIGHT:
        break;
    }
    // Everytime a new command is received we write to the goat

    // TODO: Implement the logic for the sensors driver

    // kalman checks if we are static for calibration
    kalman.check_static(dump);
    return;
}

// TODO: must return bool to written into goat.event
void Sensors::calibrate()
{
    bool calibrated(true);

    // must have counter ro return an error if too much
    // Redo calibration
    adxl1.calibrate();
    adxl2.calibrate();

    try
    {
        ina_lpb.setMaxCurrentShunt(INA228_LPB_MAX_CUR, INA228_LPB_SHUNT);
        ina_hpb.setMaxCurrentShunt(INA228_HPB_MAX_CUR, INA228_HPB_SHUNT);
    }
    catch (INA228Exception &e)
    {
        std::cout << "INA228 calibration error: " << e.what() << "\n";
        calibrated = false;
    }

    Data::get_instance().write(Data::EVENT_CALIBRATED, &calibrated);
}

bool Sensors::update()
{
    update_status();
    if (simulation_mode && tdb != nullptr)
    {
        adxl375_data adxl1_data = {
            adxl1_x.value().get(),
            adxl1_y.value().get(),
            adxl1_z.value().get()};
        adxl1_data = adxl1.add_noise_to_data(adxl1_data, 0.5);
        Data::get_instance().write(Data::NAV_SENSOR_ADXL1_DATA, &adxl1_data);

        adxl375_data adxl2_data = {
            adxl2_x.value().get(),
            adxl2_y.value().get(),
            adxl2_z.value().get()};
        adxl2_data = adxl2.add_noise_to_data(adxl2_data, 0.5);
        Data::get_instance().write(Data::NAV_SENSOR_ADXL2_DATA, &adxl2_data);

        bmi08_sensor_data_f bmi1_acc_data = {
            bmi1_acc_x.value().get(),
            bmi1_acc_y.value().get(),
            bmi1_acc_z.value().get()};
        bmi1_acc_data = bmi1.add_noise_to_data(bmi1_acc_data, 0.5);
        Data::get_instance().write(Data::NAV_SENSOR_BMI1_ACCEL_DATA, &bmi1_acc_data);

        bmi08_sensor_data_f bmi1_gyro_data = {
            bmi1_gyro_x.value().get(),
            bmi1_gyro_y.value().get(),
            bmi1_gyro_z.value().get()};
        bmi1_gyro_data = bmi1.add_noise_to_data(bmi1_gyro_data, 0.5);
        Data::get_instance().write(Data::NAV_SENSOR_BMI1_GYRO_DATA, &bmi1_gyro_data);

        bmi08_sensor_data_f bmi2_acc_data = {
            bmi2_acc_x.value().get(),
            bmi2_acc_y.value().get(),
            bmi2_acc_z.value().get()};

        bmi1_acc_data = bmi2.add_noise_to_data(bmi2_acc_data, 0.5);
        Data::get_instance().write(Data::NAV_SENSOR_BMI2_ACCEL_DATA, &bmi2_acc_data);

        bmi08_sensor_data_f bmi2_gyro_data = {
            bmi2_gyro_x.value().get(),
            bmi2_gyro_y.value().get(),
            bmi2_gyro_z.value().get()};
        bmi2_gyro_data = bmi2.add_noise_to_data(bmi2_gyro_data, 0.5);
        Data::get_instance().write(Data::NAV_SENSOR_BMI2_GYRO_DATA, &bmi2_gyro_data);

        bmp3_data bmp1_data = {
            bmp1_p.value().get(),
            bmp1_t.value().get()};
        bmp1_data = bmp1.add_noise_to_data(bmp1_data, 0.5); 
        Data::get_instance().write(Data::NAV_SENSOR_BMP1_DATA, &bmp1_data);

        bmp3_data bmp2_data = {
            bmp2_p.value().get(),
            bmp2_t.value().get()};
        bmp2_data = bmp2.add_noise_to_data(bmp2_data, 0.5);
        Data::get_instance().write(Data::NAV_SENSOR_BMP2_DATA, &bmp2_data);

        NavSensors nav_sensors{};
        nav_sensors.adxl = adxl1_data;
        nav_sensors.adxl_aux = adxl2_data;
        nav_sensors.bmi_accel = bmi1_acc_data;
        nav_sensors.bmi_gyro = bmi1_gyro_data;
        nav_sensors.bmi_aux_accel = bmi2_acc_data;
        nav_sensors.bmi_aux_gyro = bmi2_gyro_data;
        nav_sensors.bmp = bmp1_data;
        nav_sensors.bmp_aux = bmp2_data;

        kalman.predict(nav_sensors, Data::get_instance().get().nav);
        kalman.update(nav_sensors, Data::get_instance().get().nav);

        auto temp_nav_data = kalman.get_nav_data();
        Data::get_instance().write(Data::NAV_KALMAN_DATA, &temp_nav_data);

        return true;
    }

    try
    {
        float lpb_voltage(ina_lpb.getBusVoltage());
        Data::get_instance().write(Data::BAT_LPB_VOLTAGE, &lpb_voltage);
    }
    catch (INA228Exception &e)
    {
        std::cout << "INA228 LPB: " << e.what() << "\n";
    }
    try
    {
        float hpb_voltage(ina_hpb.getBusVoltage());
        Data::get_instance().write(Data::BAT_HPB_VOLTAGE, &hpb_voltage);
    }
    catch (INA228Exception &e)
    {
        std::cout << "INA228 HPB: " << e.what() << "\n";
    }

    try
    {
        float fc_temperature(tmp1075.getTemperatureCelsius());
        Data::get_instance().write(Data::AV_FC_TEMPERATURE, &fc_temperature);
    }
    catch (TMP1075Exception &e)
    {
        std::cout << e.what() << "\n";
    }

    struct NavSensors nav_sensors;

    // Update raw sensors values and write them to the GOAT
    auto temp_adxl(adxl1.get_data());
    nav_sensors.adxl = temp_adxl;
    Data::get_instance().write(Data::NAV_SENSOR_ADXL1_DATA, &temp_adxl);
    temp_adxl = adxl2.get_data();
    nav_sensors.adxl_aux = temp_adxl;
    Data::get_instance().write(Data::NAV_SENSOR_ADXL2_DATA, &temp_adxl);
    auto temp_bmi(bmi1.get_accel_data());
    nav_sensors.bmi_accel = temp_bmi;
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_ACCEL_DATA, &temp_bmi);
    temp_bmi = bmi1.get_gyro_data();
    nav_sensors.bmi_gyro = temp_bmi;
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_GYRO_DATA, &temp_bmi);
    temp_bmi = bmi2.get_accel_data();
    nav_sensors.bmi_aux_accel = temp_bmi;
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_ACCEL_DATA, &temp_bmi);
    temp_bmi = bmi2.get_gyro_data();
    nav_sensors.bmi_aux_gyro = temp_bmi;
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_GYRO_DATA, &temp_bmi);
    auto temp_bmp(bmp1.get_sensor_data());
    nav_sensors.bmp = temp_bmp;
    Data::get_instance().write(Data::NAV_SENSOR_BMP1_DATA, &temp_bmp);
    temp_bmp = bmp2.get_sensor_data();
    nav_sensors.bmp_aux = temp_bmp;
    Data::get_instance().write(Data::NAV_SENSOR_BMP2_DATA, &temp_bmp);

    // TODO: Propulsion sensors acquisition

    while (i2cgps.available())
    {
        gps.encode(i2cgps.read());
    }
    if (gps.time.isUpdated())
    {
        if (gps.date.isValid())
        {
            unsigned temp(gps.date.year());
            Data::get_instance().write(Data::NAV_GNSS_TIME_YEAR, &temp);
            temp = gps.date.month();
            Data::get_instance().write(Data::NAV_GNSS_TIME_MONTH, &temp);
            temp = gps.date.day();
            Data::get_instance().write(Data::NAV_GNSS_TIME_DAY, &temp);
        }
        if (gps.time.isValid())
        {
            unsigned temp(gps.time.hour());
            Data::get_instance().write(Data::NAV_GNSS_TIME_HOUR, &temp);
            temp = gps.time.minute();
            Data::get_instance().write(Data::NAV_GNSS_TIME_MINUTE, &temp);
            temp = gps.time.second();
            Data::get_instance().write(Data::NAV_GNSS_TIME_SECOND, &temp);
            temp = gps.time.centisecond();
            Data::get_instance().write(Data::NAV_GNSS_TIME_CENTI, &temp);
        }
        if (gps.location.isValid())
        {
            double temp(gps.location.lat());
            Data::get_instance().write(Data::NAV_GNSS_POS_LAT, &temp);
            temp = gps.location.lng();
            Data::get_instance().write(Data::NAV_GNSS_POS_LNG, &temp);
        }
        if (gps.altitude.isValid())
        {
            double temp(gps.altitude.meters());
            Data::get_instance().write(Data::NAV_GNSS_POS_ALT, &temp);
        }
        if (gps.course.isValid())
        {
            double temp(gps.course.deg());
            Data::get_instance().write(Data::NAV_GNSS_COURSE, &temp);
        }
    }

    // Kalmann filter
    kalman.predict(nav_sensors, Data::get_instance().get().nav);
    kalman.update(nav_sensors, Data::get_instance().get().nav);

    auto temp_nav_data(kalman.get_nav_data());
    Data::get_instance().write(Data::NAV_KALMAN_DATA, &temp_nav_data);

    return true;
}

void Sensors::update_status()
{
    uint8_t temp(adxl1.get_status());
    Data::get_instance().write(Data::NAV_SENSOR_ADXL1_STAT, &temp);
    temp = adxl2.get_status();
    Data::get_instance().write(Data::NAV_SENSOR_ADXL2_STAT, &temp);
    temp = bmi1.get_accel_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_ACCEL_STAT, &temp);
    temp = bmi2.get_accel_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_ACCEL_STAT, &temp);
    temp = bmi1.get_gyro_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI1_GYRO_STAT, &temp);
    temp = bmi2.get_gyro_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMI2_GYRO_STAT, &temp);

    auto temp_bmp(bmp1.get_status());
    Data::get_instance().write(Data::NAV_SENSOR_BMP1_STAT, &temp_bmp);
    temp_bmp = bmp2.get_status();
    Data::get_instance().write(Data::NAV_SENSOR_BMP2_STAT, &temp_bmp);
}
