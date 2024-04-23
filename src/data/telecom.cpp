#include <LoRa.h>
#include "telecom.h"

#define lora_uplink LoRa

constexpr unsigned long LORA_FREQUENCY(868e6);

namespace {
    void handle_uplink(int packet_size);
    void handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint32_t len);
    void handle_downlink(int packet_size);
    void handle_capule_downlink(uint8_t packet_id, uint8_t* data_in, uint32_t len);

    CapsuleStatic capsule_uplink(handle_capsule_uplink);
    CapsuleStatic capsule_downlink(handle_capule_downlink);

    LoRaClass lora_downlink;
}

Telecom::Telecom() : new_cmd_received(false), last_packet{0, 0} {}

Telecom::~Telecom() {}

void Telecom::begin() {
    lora_uplink.setPins(/*UPLINK_SS_PIN, UPLINK_RST_PIN, UPLINK_DIO0_PIN*/);
    if (!lora_uplink.begin(LORA_FREQUENCY, SPI0)) {
        std::cout << "LoRa uplink init failed!\n";
    }else {
        std::cout << "LoRa uplink init succeeded!\n";
    }

    lora_downlink.setPins(/*DOWNLINK_SS_PIN, DOWNLINK_RST_PIN, DOWNLINK_DIO0_PIN*/);
    if (!lora_uplink.begin(LORA_FREQUENCY, SPI1)) {
        std::cout << "LoRa downlink init failed!\n";
    }else {
        std::cout << "LoRa downlink init succeeded!\n";
    }
    lora_uplink.receive();

}

void Telecom::update() {
    /* TODO */
}

UpLink Telecom::get_cmd() const {
    UpLink out;
    out.id = static_cast<CMD_ID>(last_packet.order_id);
    out.value = last_packet.order_value;
    return out;
}

void Telecom::reset_cmd() {
    last_packet = {0, 0};
}