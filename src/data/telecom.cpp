#include <LoRa.h>
#include <LoopbackStream.h>
#include <Protocol.h>
#include <capsule.h>
#include "telecom.h"
#include "data.h"

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

#if (UPLINK_CRC)
    lora_uplink.enableCrc();
#else
    lora_uplink.disableCrc();
#endif
#if (UPLINK_INVERSE_IQ)
    lora_uplink.enableInvertIQ();
#else
    lora_uplink.disableInvertIQ();
#endif

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

#if (UPLINK_CRC)
    lora_downlink.enableCrc();
#else
    lora_downlink.disableCrc();
#endif
#if (UPLINK_INVERSE_IQ)
    lora_downlink.enableInvertIQ();
#else
    lora_downlink.disableInvertIQ();
#endif

    return true;
}

//implement policy()
//TODO: this is an example

void Telecom::policy() {
    if (new_cmd_received) {
        new_cmd_received = false;
        switch (last_packet.order_id) {
            case CMD_ID::AV_CMD_ABORT:
                reset_cmd();
                break;
            default:
                break;
        }
    }
}

void Telecom::send_telemetry() {
    const DataDump data = Data::get_instance().get();

    av_downlink_t packet;
    packet.gnss_lat = data.nav.position.lat;
    packet.gnss_lon = data.nav.position.lng;
    packet.gnss_alt = data.nav.position.alt;

    packet.N2_pressure = data.prop.N2_pressure;
    packet.fuel_pressure = data.prop.fuel_pressure;
    packet.LOX_pressure = data.prop.LOX_pressure;
    packet.igniter_pressure = data.prop.igniter_pressure;
    packet.LOX_inj_pressure = data.prop.LOX_inj_pressure;
    packet.fuel_inj_pressure = data.prop.fuel_inj_pressure;
    packet.chamber_pressure = data.prop.chamber_pressure;
    packet.fuel_level = data.prop.fuel_level;
    packet.LOX_level = data.prop.LOX_level;
    packet.N2_temp = data.prop.N2_temperature;
    packet.fuel_temp = data.prop.fuel_temperature;
    packet.LOX_temp = data.prop.LOX_temperature;
    packet.igniter_temp = data.prop.igniter_temperature;
    packet.fuel_inj_temp = data.prop.fuel_inj_temperature;
    packet.fuel_inj_cool_temp = data.prop.fuel_inj_cooling_temperature;
    packet.LOX_inj_temp = data.prop.LOX_inj_temperature;
    packet.engine_temp = data.prop.chamber_temperature;

    send_packet(CAPSULE_ID::AV_TELEMETRY, (uint8_t*)&packet, av_downlink_size);
}

void Telecom::update() {
    while (uplink_buffer.available()) {
        capsule_uplink.decode(uplink_buffer.read());
    }
    while (downlink_buffer.available()) {
        capsule_downlink.decode(downlink_buffer.read());
    }
}

void Telecom::reset_cmd() {
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

void Telecom::handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    switch (packet_id) {
        case CAPSULE_ID::GS_CMD:
            memcpy(&last_packet, data_in, len);
            new_cmd_received = true;

            Data::get_instance().write(Data::TLM_CMD_ID, &last_packet.order_id);
            Data::get_instance().write(Data::TLM_CMD_VALUE, &last_packet.order_value);

            std::cout << "Command received from GS!\n"
                      << "ID: " << (int)last_packet.order_id << "\n"
                      << "Value: " << (int)last_packet.order_value << "\n\n";
            break;
    }
}

void Telecom::handle_downlink(int packet_size) {
    for (int i(0); i < packet_size; ++i) {
        downlink_buffer.write(lora_downlink.read());
    }
}

void Telecom::handle_capsule_downlink(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    std::cerr << "Packet received unexpectedly on the downlink radio: \n"
              << "ID: " << packet_id << "\n"
              << "Data: ";
    for (uint16_t i(0); i < len; ++i) {
        std::cerr << data_in[i];
    }
    std::cerr << "\n";
}

void Telecom::send_packet(uint8_t packet_id, uint8_t* data, uint32_t len) {
    uint8_t* coded_buffer(capsule_downlink.encode(packet_id, data, len));
    size_t length(capsule_downlink.getCodedLen(len));

    lora_downlink.beginPacket();
    lora_downlink.write(coded_buffer, length);
    lora_downlink.endPacket(true);

    delete[] coded_buffer;
}
