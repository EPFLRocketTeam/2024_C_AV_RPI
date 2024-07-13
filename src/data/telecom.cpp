#include <LoRa.h>
#include <capsule.h>
#include <LoopbackStream.h>
#include <ERT_RF_Protocol_Interface/ParameterDefinition.h>
#include "telecom.h"

#define LORA_UPLINK_CS      8
#define LORA_UPLINK_RST     25
#define LORA_UPLINK_DI0     5

#define LORA_DOWNLINK_CS    7
#define LORA_DOWNLINK_RST   24
#define LORA_DOWNLINK_DI0   6

#define lora_uplink LoRa

namespace {
    LoRaClass lora_downlink;

    LoopbackStream uplink_buffer(MAX_BUFFER_SIZE);
    LoopbackStream downlink_buffer(MAX_BUFFER_SIZE);
}


Telecom::Telecom() 
:   new_cmd_received(false),
    last_packet{0, 0},
    capsule_uplink(&Telecom::handle_capsule_uplink, this),
    capsule_downlink(&Telecom::handle_capsule_downlink, this)
{}

bool Telecom::begin() {
    lora_uplink.setPins(LORA_UPLINK_CS, LORA_UPLINK_RST, LORA_UPLINK_DI0);
    if (!lora_uplink.begin(UPLINK_FREQUENCY, SPI0)) {
        std::cout << "LoRa uplink init failed!\n";
        return false;
    }else {
        std::cout << "LoRa uplink init succeeded!\n";
    }

    lora_uplink.setTxPower(UPLINK_POWER);
    lora_uplink.setSignalBandwidth(UPLINK_BW);
    lora_uplink.setSpreadingFactor(UPLINK_SF);
    lora_uplink.setCodingRate4(UPLINK_CR);
    lora_uplink.setPreambleLength(UPLINK_PREAMBLE_LEN);

    // Set uplink radio as a continuous receiver
    lora_uplink.receive();
    lora_uplink.onReceive(handle_uplink);

    lora_downlink.setPins(LORA_DOWNLINK_CS, LORA_DOWNLINK_RST, LORA_DOWNLINK_DI0);
    if (!lora_downlink.begin(AV_DOWNLINK_FREQUENCY, SPI1)) {
        std::cout << "LoRa downlink init failed!\n";
        return false;
    }else {
        std::cout << "LoRa downlink init succeeded!\n";
    }

    lora_downlink.setTxPower(AV_DOWNLINK_POWER);
    lora_downlink.setSignalBandwidth(AV_DOWNLINK_BW);
    lora_downlink.setSpreadingFactor(AV_DOWNLINK_SF);
    lora_downlink.setCodingRate4(AV_DOWNLINK_CR);
    lora_downlink.setPreambleLength(AV_DOWNLINK_PREAMBLE_LEN);

    return true;
}

void Telecom::send_packet(uint8_t packet_id, uint8_t* data, uint16_t len) {
    av_downlink_t packet;
    memcpy(&packet, data, len);

    uint8_t* coded_buffer(capsule_downlink.encode(packet_id, data, len));
    size_t length(capsule_downlink.getCodedLen(len));

    lora_downlink.beginPacket();
    lora_downlink.write(coded_buffer, length);
    lora_downlink.endPacket(true);

    std::cout << "Sent packet of size " << length << "\n";

    delete[] coded_buffer;
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
    if (packet_size == 0) {
        return;
    }

    std::cout << "packet received\n";

    for (int i(0); i < packet_size; ++i) {
        uplink_buffer.write(lora_uplink.read());
    }
}

void Telecom::handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint16_t len) {
    switch (packet_id) {
        case CAPSULE_ID::GS_CMD:
            memcpy(&last_packet, data_in, len);
            new_cmd_received = true;
            std::cout << "Command received from GS!\n"
                      << "ID: " << last_packet.order_id << "\n"
                      << "Value: " << last_packet.order_value << "\n";
        break;
    }
}

void Telecom::handle_downlink(int packet_size) {
    for (int i(0); i < packet_size; ++i) {
        downlink_buffer.write(lora_downlink.read());
    }
}

void Telecom::handle_capsule_downlink(uint8_t packet_id, uint8_t* data_in, uint16_t len) {
    std::cerr << "Packet received unexpectedly on the downlink radio: \n"
              << "ID: " << packet_id << "\n"
              << "Data: ";
    for (uint16_t i(0); i < len; ++i) {
        std::cerr << data_in[i];
    }
    std::cerr << "\n";
}