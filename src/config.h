#ifndef CONFIG_H
#define CONFIG_H

/* PINOUTS / CONFIGS */
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

/* TIMINGS */
#define MAIN_LOOP_MAX_FREQUENCY  100
#define IGNITION_SEND_TIMEOUT_MS 5000
#define IGNITION_ACK_DELAY_MS    500  // TO TUNE DEPENDING ON THE ACTUAL DELAY ON IGNITION SEQUENCE

/* LOGGING */
#define LOG_DUMP_DEFAULT_PATH   "/boot/av_log/dump_log.log"
#define LOG_EVENT_DEFAULT_PATH  "/boot/av_log/event_log.log"
#define DATADUMP_FSYNC_FREQ     50

extern const bool CONSOLE_LOG;
extern const bool DEBUG_LOG;

#endif /* CONFIG_H */
