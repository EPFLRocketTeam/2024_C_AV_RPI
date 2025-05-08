#include "sensors.h"
#include "INA228.h"
#include "TMP1075.h"
#include "kalman_params.h"
#include "config.h"
#include "data.h"

#define SENSOR_WRITE(flag, opt_sensor, method_call, id, varname)         \
    do {                                                                 \
        if ((flag) && (opt_sensor)) {                                    \
            auto varname = (opt_sensor)->method_call;                    \
            Data::get_instance().write((id), &varname);                  \
        }                                                                \
    } while (0)




Sensors::Sensors():  kalman(INITIAL_COV_GYR_BIAS,
    INITIAL_COV_ACCEL_BIAS,
    INITIAL_COV_ORIENTATION,
    GYRO_COV,
    GYRO_BIAS_COV,
    ACCEL_COV,
    ACCEL_BIAS_COV,
    GPS_OBS_COV,
    ALT_OBS_COV){
        #if USE_ADXL1
        try {
            adxl1.emplace(ADXL375_ADDR_I2C_PRIM);
        } catch (...) {
            std::cout << "ADXL1 init failed\n";
            adxl1.reset();
        }
    #endif
    
    #if USE_ADXL2
        try {
            adxl2.emplace(ADXL375_ADDR_I2C_SEC);
        } catch (...) {
            std::cout << "ADXL2 init failed\n";
            adxl2.reset();
        }
    #endif
    
    #if USE_BMI1
        try {
            bmi1.emplace(BMI08_ACCEL_I2C_ADDR_PRIMARY, BMI08_GYRO_I2C_ADDR_PRIMARY);
        } catch (...) {
            std::cout << "BMI1 init failed\n";
            bmi1.reset();
        }
    #endif
    
    #if USE_BMI2
        try {
            bmi2.emplace(BMI08_ACCEL_I2C_ADDR_SECONDARY, BMI08_GYRO_I2C_ADDR_SECONDARY);
        } catch (...) {
            std::cout << "BMI2 init failed\n";
            bmi2.reset();
        }
    #endif
    
    #if USE_BMP1
        try {
            bmp1.emplace(BMP3_ADDR_I2C_PRIM);
        } catch (...) {
            std::cout << "BMP1 init failed\n";
            bmp1.reset();
        }
    #endif
    
    #if USE_BMP2
        try {
            bmp2.emplace(BMP3_ADDR_I2C_SEC);
        } catch (...) {
            std::cout << "BMP2 init failed\n";
            bmp2.reset();
        }
    #endif
    
    #if USE_I2CGPS
        try {
            i2cgps.emplace();
        } catch (...) {
            std::cout << "I2CGPS init failed\n";
            i2cgps.reset();
        }
    #endif
    
    #if USE_GPS
        try {
            gps.emplace();
        } catch (...) {
            std::cout << "GPS init failed\n";
            gps.reset();
        }
    #endif
    
    #if USE_TMP1075
        try {
            tmp1075.emplace(TMP1075_ADDR_I2C);
        } catch (...) {
            std::cout << "TMP1075 init failed\n";
            tmp1075.reset();
        }
    #endif
    
    #if USE_INA_LPB
        try {
            ina_lpb.emplace(INA228_ADDRESS_LPB, INA228_LPB_SHUNT, INA228_LPB_MAX_CUR);
            if (!ina_lpb->isConnected()) {
                std::cout << "INA LPB not connected\n";
                ina_lpb.reset();
            }
        } catch (const INA228Exception& e) {
            std::cout << "INA LPB exception: " << e.what() << "\n";
            ina_lpb.reset();
        } catch (...) {
            std::cout << "INA LPB unknown error\n";
            ina_lpb.reset();
        }
    #endif
    
    #if USE_INA_HPB
        try {
            ina_hpb.emplace(INA228_ADDRESS_HPB, INA228_HPB_SHUNT, INA228_HPB_MAX_CUR);
            if (!ina_hpb->isConnected()) {
                std::cout << "INA HPB not connected\n";
                ina_hpb.reset();
            }
        } catch (const INA228Exception& e) {
            std::cout << "INA HPB exception: " << e.what() << "\n";
            ina_hpb.reset();
        } catch (...) {
            std::cout << "INA HPB unknown error\n";
            ina_hpb.reset();
        }
    #endif
    
        update_status();  // Optional: only if you want initial sensor status tracking
}

Sensors::~Sensors() {}

void Sensors::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    switch (dump.av_state) {
        case State::INIT:
        case State::MANUAL:
        case State::ARMED:
            // TODO: low polling rate
            // update();
            break;
        case State::CALIBRATION:
            // TODO: calibration + low polling rate
            // update();
            if (!dump.event.calibrated) {
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

//TODO: must return bool to written into goat.event
void Sensors::calibrate() {
    bool calibrated(true);

    //must have counter ro return an error if too much
    //Redo calibration
    //adxl1.calibrate();
    //adxl2.calibrate();

    try {
        #if USE_INA_HPB
            ina_hpb->setMaxCurrentShunt(INA228_HPB_MAX_CUR, INA228_HPB_SHUNT);
        #endif
        #if USE_INA_LPB
            ina_lpb->setMaxCurrentShunt(INA228_LPB_MAX_CUR, INA228_LPB_SHUNT);
        #endif

    }catch(INA228Exception& e) {
        std::cout << "INA228 calibration error: " << e.what() << "\n";
        calibrated = false;
    }

    Data::get_instance().write(Data::EVENT_CALIBRATED, &calibrated);
}
bool Sensors::update() {
    update_status();



#if USE_INA_LPB
    if (ina_lpb) {
        try {
            SENSOR_WRITE(USE_INA_LPB, ina_lpb, getBusVoltage(), Data::BAT_LPB_VOLTAGE,ina_lpb_voltage);
        } catch (INA228Exception& e) {
            std::cout << "INA228 LPB error: " << e.what() << "\n";
        }
    }
#endif

#if USE_INA_HPB
    if (ina_hpb) {
        try {
            SENSOR_WRITE(USE_INA_HPB, ina_hpb, getBusVoltage(), Data::BAT_HPB_VOLTAGE,ina_hpb_voltage);
        } catch (INA228Exception& e) {
            std::cout << "INA228 HPB error: " << e.what() << "\n";
        }
    }
#endif

#if USE_TMP1075
    if (tmp1075) {
        try {
            SENSOR_WRITE(USE_TMP1075, tmp1075, getTemperatureCelsius(), Data::AV_FC_TEMPERATURE,tmp1075_temp);
        } catch (TMP1075Exception& e) {
            std::cout << e.what() << "\n";
        }
    }
#endif

#if USE_ADXL1
    SENSOR_WRITE(USE_ADXL1, adxl1, get_data(), Data::NAV_SENSOR_ADXL1_DATA,adxl1_data);
#endif

#if USE_ADXL2
    SENSOR_WRITE(USE_ADXL2, adxl2, get_data(), Data::NAV_SENSOR_ADXL2_DATA,adxl2_data);
#endif

#if USE_BMI1
    SENSOR_WRITE(USE_BMI1, bmi1, get_accel_data(), Data::NAV_SENSOR_BMI1_ACCEL_DATA,bmi1_accel_data);
    SENSOR_WRITE(USE_BMI1, bmi1, get_gyro_data(),  Data::NAV_SENSOR_BMI1_GYRO_DATA,bmi1_gyro_data);
#endif

#if USE_BMI2
    SENSOR_WRITE(USE_BMI2, bmi2, get_accel_data(), Data::NAV_SENSOR_BMI2_ACCEL_DATA, bmi2_accel_data);
    SENSOR_WRITE(USE_BMI2, bmi2, get_gyro_data(),  Data::NAV_SENSOR_BMI2_GYRO_DATA, bmi2_gyro_data);
#endif

#if USE_BMP1
    SENSOR_WRITE(USE_BMP1, bmp1, get_sensor_data(), Data::NAV_SENSOR_BMP1_DATA, bmp1_data);
#endif

#if USE_BMP2
    SENSOR_WRITE(USE_BMP2, bmp2, get_sensor_data(), Data::NAV_SENSOR_BMP2_DATA, bmp2_data);
#endif


#if USE_KALMAN
    NavSensors nav_sensors;
    if (adxl1) nav_sensors.adxl = adxl1->get_data();
    if (adxl2) nav_sensors.adxl_aux = adxl2->get_data();
    if (bmi1) {
        nav_sensors.bmi_accel = bmi1->get_accel_data();
        nav_sensors.bmi_gyro  = bmi1->get_gyro_data();
    }
    if (bmi2) {
        nav_sensors.bmi_aux_accel = bmi2->get_accel_data();
        nav_sensors.bmi_aux_gyro  = bmi2->get_gyro_data();
    }
    if (bmp1) nav_sensors.bmp = bmp1->get_sensor_data();
    if (bmp2) nav_sensors.bmp_aux = bmp2->get_sensor_data();

    kalman.predict(nav_sensors, Data::get_instance().get().nav);
    kalman.update(nav_sensors, Data::get_instance().get().nav);

    auto nav = kalman.get_nav_data();
    Data::get_instance().write(Data::NAV_KALMAN_DATA, &nav);
#endif
return true;
}



void Sensors::update_status() {
    #if USE_ADXL1
        if (adxl1) {
            uint8_t temp = adxl1->get_status();
            Data::get_instance().write(Data::NAV_SENSOR_ADXL1_STAT, &temp);
        }
    #endif
    
    #if USE_ADXL2
        if (adxl2) {
            uint8_t temp = adxl2->get_status();
            Data::get_instance().write(Data::NAV_SENSOR_ADXL2_STAT, &temp);
        }
    #endif
    
    #if USE_BMI1
        if (bmi1) {
            uint8_t accel_stat = bmi1->get_accel_status();
            Data::get_instance().write(Data::NAV_SENSOR_BMI1_ACCEL_STAT, &accel_stat);
    
            uint8_t gyro_stat = bmi1->get_gyro_status();
            Data::get_instance().write(Data::NAV_SENSOR_BMI1_GYRO_STAT, &gyro_stat);
        }
    #endif
    
    #if USE_BMI2
        if (bmi2) {
            uint8_t accel_stat = bmi2->get_accel_status();
            Data::get_instance().write(Data::NAV_SENSOR_BMI2_ACCEL_STAT, &accel_stat);
    
            uint8_t gyro_stat = bmi2->get_gyro_status();
            Data::get_instance().write(Data::NAV_SENSOR_BMI2_GYRO_STAT, &gyro_stat);
        }
    #endif
    
    #if USE_BMP1
        if (bmp1) {
            auto status = bmp1->get_status();
            Data::get_instance().write(Data::NAV_SENSOR_BMP1_STAT, &status);
        }
    #endif
    
    #if USE_BMP2
        if (bmp2) {
            auto status = bmp2->get_status();
            Data::get_instance().write(Data::NAV_SENSOR_BMP2_STAT, &status);
        }
    #endif
    }
    
