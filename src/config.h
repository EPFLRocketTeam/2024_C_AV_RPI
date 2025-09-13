#ifndef CONFIG_H
#define CONFIG_H

// ABORT CONFIG  - DO NOT TOUCH
#define ABORT_FLIGHT_EN     0
///////////////////////////////

/* Frequencies */
#define MAIN_LOOP_MAX_FREQUENCY  100
#define DATADUMP_FSYNC_FREQUENCY 50

/* Logger */
#define LOG_DUMP_DEFAULT_PATH   "/boot/av_log/dump_log.log"
#define LOG_EVENT_DEFAULT_PATH  "/boot/av_log/event_log.log"
extern const bool CONSOLE_LOG;
extern const bool DEBUG_LOG;

/* GPIOS */
#define LORA_UPLINK_CS      8
#define LORA_UPLINK_RST     24
#define LORA_UPLINK_DI0     5

#define LORA_DOWNLINK_CS    7
#define LORA_DOWNLINK_RST   25
#define LORA_DOWNLINK_DI0   6

#define BUZZER              4

#define LED_STATUS          13
#define LED_LORA_RX         17
#define LED_LORA_TX         18

/* Sensors */
#define INA228_LPB_SHUNT       65.2e-3
#define INA228_LPB_MAX_CUR     2.5
#define INA228_HPB_TRB_SHUNT   27e-3
#define INA228_HPB_TRB_MAX_CUR 6
#define INA228_HPB_PRB_SHUNT   13e-3
#define INA228_HPB_PRB_MAX_CUR 12

#define TMP1075_ALERT_TEMPERATURE 55

#endif /* CONFIG_H */
