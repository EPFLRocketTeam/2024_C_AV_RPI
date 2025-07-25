// TODO: log events instead of std::cout

#include <LoRa.h>
#include <LoopbackStream.h>
#include <Protocol.h>
#include <capsule.h>
#include <pigpio.h>
#include "DownlinkCompression_Firehorn.h"
#include "PacketDefinition_Firehorn.h"
#include "telecom.h"
#include "data.h"
#include "h_driver.h"
#include "config.h"

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

void Telecom::check_policy(const DataDump& dump, const uint32_t delta_ms) {
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

    // Policy regarding downlink
    switch (dump.av_state) {
        case State::INIT:
            // TODO: try establishing communication with GS
            break;
        default:
            send_telemetry();
            break;
    }

    // Write incoming packets to buffer
    // TODO: see if this can be called in handle_uplink during callback instead of polling
    update();
}

bool Telecom::begin() {
    lora_uplink.setPins(LORA_UPLINK_CS, LORA_UPLINK_RST, LORA_UPLINK_DI0);
    if (!lora_uplink.begin(UPLINK_FREQUENCY, SPI0)) {
        throw TelecomException("LoRa uplink init failed\n");
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
        throw TelecomException("LoRa downlink init failed\n");
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

void Telecom::send_telemetry() {
    const DataDump data = Data::get_instance().get();

    av_downlink_unpacked packet;
    packet.packet_nbr = data.av_timestamp;
    packet.gnss_lon = data.nav.position.lng;
    packet.gnss_lat = data.nav.position.lat;
    packet.gnss_alt = data.nav.position.alt;
    packet.gnss_vertical_speed = 0;
    
    packet.N2_pressure = data.prop.N2_pressure;
    packet.fuel_pressure = data.prop.fuel_pressure;
    packet.LOX_pressure = data.prop.LOX_pressure;
    packet.fuel_level = data.prop.fuel_level;
    packet.LOX_level = data.prop.LOX_level;
    packet.N2_temp = data.prop.N2_temperature;
    packet.LOX_temp = data.prop.LOX_temperature;
    packet.LOX_inj_temp = data.prop.LOX_inj_temperature;

    packet.lpb_voltage = data.bat.lpb_voltage;
    packet.hpb_voltage = data.bat.hpb_voltage;

    packet.av_fc_temp = data.av_fc_temp;
    packet.ambient_temp = 0;
    packet.engine_state = 0;

    packet.av_state = (uint8_t)data.av_state;

    packet.cam_rec = data.cams_recording.cam_sep * CAMERA_REC_PARACHUTE
                     | data.cams_recording.cam_up * CAMERA_REC_AIRFRAME_UP
                     | data.cams_recording.cam_down * CAMERA_REC_AIRFRAME_DN;
    
    av_downlink_t compressed_packet(encode_downlink(packet));

    send_packet(CAPSULE_ID::AV_TELEMETRY, (uint8_t*)&compressed_packet, av_downlink_size);
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
            gpioWrite(LED_LORA_RX, 1);

            memcpy(&last_packet, data_in, len);
            new_cmd_received = true;

            Data::get_instance().write(Data::TLM_CMD_ID, &last_packet.order_id);
            Data::get_instance().write(Data::TLM_CMD_VALUE, &last_packet.order_value);
            Data::get_instance().write(Data::EVENT_CMD_RECEIVED, &new_cmd_received);

            std::cout << "Command received from GS!\n"
                      << "ID: " << (int)last_packet.order_id << "\n"
                      << "Value: " << (int)last_packet.order_value << "\n\n";

            gpioWrite(LED_LORA_RX, 0);
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
    gpioWrite(LED_LORA_TX, 1);

    uint8_t* coded_buffer(capsule_downlink.encode(packet_id, data, len));
    size_t length(capsule_downlink.getCodedLen(len));

    lora_downlink.beginPacket();
    lora_downlink.write(coded_buffer, length);
    lora_downlink.endPacket(true);

    delete[] coded_buffer;

    gpioWrite(LED_LORA_TX, 0);
}
