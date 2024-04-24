#include <LoRa.h>
#include <capsule.h>
#include <LoopbackStream.h>
#include "telecom.h"

#define lora_uplink LoRa

constexpr unsigned long LORA_FREQUENCY(868e6);

namespace {
    LoRaClass lora_downlink;

    LoopbackStream uplink_buffer(MAX_BUFFER_SIZE);
    LoopbackStream downlink_buffer(MAX_BUFFER_SIZE);
}


Telecom::Telecom() 
:   new_cmd_received(false),
    last_packet{0, 0},
    capsule_uplink(Telecom::handle_capsule_uplink, this),
    capsule_downlink(Telecom::handle_capsule_downlink, this)
{}

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
    lora_uplink.onReceive(handle_uplink);
}

void Telecom::send_packet(uint8_t packet_id, uint8_t* data, uint16_t len) {
    av_downlink_t packet;
    memcpy(&packet, data, len);

    uint8_t* buffer(capsule_downlink.encode(packet_id, data, len));
    size_t length(capsule_downlink.getCodedLen(len));

    lora_downlink.beginPacket();
    lora_downlink.write(buffer, length);
    lora_downlink.endPacket(true);

    delete[] buffer;
}

void Telecom::update() {
    while (uplink_buffer.available()) {
        capsule_uplink.decode(uplink_buffer.read());
    }
    while (downlink_buffer.available()) {
        capsule_downlink.decode(downlink_buffer.read());
    }
}

UplinkCmd Telecom::get_cmd() const {
    UplinkCmd cmd;
    cmd.id = static_cast<CMD_ID>(last_packet.order_id);
    cmd.value = last_packet.order_value;
    return cmd;
}

void Telecom::reset_cmd() {
    last_packet = {0, 0};
}

void Telecom::handle_uplink(int packet_size) {
    for (int i(0); i < packet_size; ++i) {
        uplink_buffer.write(lora_uplink.read());
    }
}

void Telecom::handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint16_t len) {
    switch (packet_id) {
        case CAPSULE_ID::GS_CMD:
            memcpy(&last_packet, data_in, len);
            new_cmd_received = true;
        break;
    }
}

void handle_downlink(int packet_size) {
    for (int i(0); i < packet_size; ++i) {
        downlink_buffer.write(lora_downlink.read());
    }
}

void handle_capsule_downlink(uint8_t packet_id, uint8_t* data_in, uint16_t len) {
    std::cerr << "Packet received unexpectedly on the downlink radio: \n"
              << "ID: " << packet_id << "\n"
              << "Data: ";
    for (uint16_t i(0); i < len; ++i) {
        std::cerr << data_in[i];
    }
    std::cerr << "\n";
}