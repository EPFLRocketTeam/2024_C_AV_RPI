#ifndef SENSORS_H
#define SENSORS_H

#define ADXL375_DATA_READY      (1 << 7)
#define BMI08_ACCEL_DATA_READY_INT              UINT8_C(0x80)
#define BMI08_GYRO_DATA_READY_INT               UINT8_C(0x80)
#define BMP3_CMD_RDY                            UINT8_C(0x10)
#define BMP3_DRDY_PRESS                         UINT8_C(0x20)
#define BMP3_DRDY_TEMP                          UINT8_C(0x40)

//#include "../include/sensor/adxl375.h"
//#include "../include/sensor/bmi08x.h"
//#include "../include/sensor/bmp3.h"

#include <stdint.h>

struct bmp3_int_status
{
    /*! Fifo watermark interrupt */
    uint8_t fifo_wm;

    /*! Fifo full interrupt */
    uint8_t fifo_full;

    /*! Data ready interrupt */
    uint8_t drdy;
};

struct bmp3_sens_status
{
    /*! Command ready status */
    uint8_t cmd_rdy;

    /*! Data ready for pressure */
    uint8_t drdy_press;

    /*! Data ready for temperature */
    uint8_t drdy_temp;
};

struct bmp3_err_status
{
    /*! Fatal error */
    uint8_t fatal;

    /*! Command error */
    uint8_t cmd;

    /*! Configuration error */
    uint8_t conf;
};

struct bmp3_status
{
    /*! Interrupt status */
    struct bmp3_int_status intr;

    /*! Sensor status */
    struct bmp3_sens_status sensor;

    /*! Error status */
    struct bmp3_err_status err;

    /*! Power on reset status */
    uint8_t pwr_on_rst;
};

struct SensStatus {
    uint8_t     adxl_status;
    uint8_t     adxl_aux_status;
    uint8_t     bmi_accel_status;
    uint8_t     bmi_aux_accel_status;
    uint8_t     bmi_gyro_status;
    uint8_t     bmi_aux_gyro_status;
    bmp3_status bmp_status;
    bmp3_status bmp_aux_status;
};;

struct adxl375_data {
	float x;
	float y;
	float z;
};

struct bmi08_sensor_data_f
{
    /*! X-axis sensor data */
    float x;

    /*! Y-axis sensor data */
    float y;

    /*! Z-axis sensor data */
    float z;
};

struct bmp3_data
{
    /*! Compensated temperature */
    double temperature;

    /*! Compensated pressure */
    double pressure;
};

struct SensRaw {
    adxl375_data        adxl; // accel
    adxl375_data        adxl_aux; // accel
    bmi08_sensor_data_f bmi_accel;
    bmi08_sensor_data_f bmi_gyro; // Correspondance x,y,z / roll,pitch,yaw à déterminer
    bmi08_sensor_data_f bmi_aux_accel;
    bmi08_sensor_data_f bmi_aux_gyro;
    bmp3_data           bmp; // temp and pressure
    bmp3_data           bmp_aux; // temp and pressure

    SensRaw();
};

struct Vector3 {
    double x;
    double y;
    double z;
};

struct SensFiltered {
    Vector3   speed;
    Vector3   accel;
    double   altitude;
    bmp3_data baro;
    double    N2_pressure;
    double    fuel_pressure;
    double    LOX_pressure;
    double    fuel_level;
    double    LOX_level;
    double    engine_temperature;
    double    igniter_pressure;
    double    LOX_inj_pressure;
    double    fuel_inj_pressure;
    double    chamber_pressure;

    SensFiltered();
};

class Adxl375 {
    private:
        uint8_t status;
        adxl375_data data;
    
    public:
        void set_value(adxl375_data value) {
	        data = value;
        }

        void set_status(uint8_t value) {
            status = value;
        }

        uint8_t get_status() {
            return status;
        }

        adxl375_data get_data() {
            return data;
        }
};

class Bmi088 {
    private:
        uint8_t accel_status;
        uint8_t gyro_status;
        bmi08_sensor_data_f accel_data;
        bmi08_sensor_data_f gyro_data;
    
    public:
        void set_accel_value(bmi08_sensor_data_f value) {
            accel_data = value;
        }

        void set_accel_status(uint8_t value) {
            accel_status = value;
        }

        void set_gyro_value(bmi08_sensor_data_f value) {
            gyro_data = value;
        }

        void set_gyro_status(uint8_t value) {
            gyro_status = value;
        }

        // uint8_t get_status() {
        //     return status;
        // }

        uint8_t get_accel_status() {
            return accel_status;
        }

        bmi08_sensor_data_f get_accel_data() {
            return accel_data;
        }

        uint8_t get_gyro_status() {
            return gyro_status;
        }

        bmi08_sensor_data_f get_gyro_data() {
            return gyro_data;
        }
};

class Bmp390 {
    private:
        bmp3_status status;
        bmp3_data data;

    public:
        void set_value(bmp3_data value) {
            data = value;
        }

        void set_status(bmp3_status value) {
            status = value;
        }

        bmp3_data get_data() {
            return data;
        }

        bmp3_status get_status() {
            return status;
        }

};

class Sensors {
public:
    Sensors();
    ~Sensors();

    //void calibrate();
    bool update();
    inline SensStatus get_status() const { return status; }
    inline SensFiltered dump() const { return clean_data; }

    // added by me
    SensRaw get_raw_data();
    SensStatus get_status();

    void set_adxl_status(uint8_t);
    void set_adxl_aux_status(uint8_t);
    void set_bmi_accel_status(uint8_t);
    void set_bmi_aux_accel_status(uint8_t);
    void set_bmi_gyro_status(uint8_t);
    void set_bmi_aux_gyro_status(uint8_t);
    void set_bmp_status(bmp3_status);
    void set_bmp_aux_status(bmp3_status);

    void set_adxl(adxl375_data);
    void set_adxl_aux(adxl375_data);
    void set_bmi_accel(bmi08_sensor_data_f);
    void set_bmi_aux_accel(bmi08_sensor_data_f);
    void set_bmi_gyro(bmi08_sensor_data_f);
    void set_bmi_aux_gyro(bmi08_sensor_data_f);
    void set_bmp(bmp3_data);
    void set_bmp_aux(bmp3_data);

    void set_altitude(double);
    void set_velocity(Vector3);
private:
    Adxl375 adxl1, adxl2;
    Bmi088 bmi1, bmi2;
    Bmp390 bmp1, bmp2;
    SensStatus status;
    SensRaw raw_data;
    SensFiltered clean_data;

    // Read sensors status
    void update_status();
};

#endif /* SENSORS_H */