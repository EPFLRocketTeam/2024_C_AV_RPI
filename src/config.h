#ifndef CONFIG_H
#define CONFIG_H

#define LORA_UPLINK_CS      8
#define LORA_UPLINK_RST     24
#define LORA_UPLINK_DI0     5

#define LORA_DOWNLINK_CS    7
#define LORA_DOWNLINK_RST   25
#define LORA_DOWNLINK_DI0   6

#define INA228_LPB_SHUNT    65.2e-3
#define INA228_LPB_MAX_CUR  2.5

#define INA228_HPB_SHUNT    27e-3
#define INA228_HPB_MAX_CUR  6

#define BUZZER 4

#define LED_STATUS  27
#define LED_LORA_RX 17
#define LED_LORA_TX 18



#define USE_ADXL1     1
#define USE_ADXL2     1
#define USE_BMI1      1
#define USE_BMI2      1
#define USE_BMP1      1
#define USE_BMP2      1
#define USE_GPS       1
#define USE_TMP1075   1
#define USE_INA_LPB   1
#define USE_INA_HPB   1
#define USE_KALMAN    1



#endif /* CONFIG_H */
