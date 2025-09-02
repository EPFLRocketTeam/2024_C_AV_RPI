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
#include "logger.h"
#include "h_driver.h"
#include "config.h"

#define lora_uplink LoRa

namespace {
    LoRaClass lora_downlink;

    LoopbackStream uplink_buffer(MAX_BUFFER_SIZE);
    LoopbackStream downlink_buffer(MAX_BUFFER_SIZE);

    bool tx_done(true);
}


Telecom::Telecom()
:   new_cmd_received(false),
    last_packet{0, 0},
    packet_number(0),
    capsule_uplink(&Telecom::handle_capsule_uplink, this),
    capsule_downlink(&Telecom::handle_capsule_downlink, this)
{}

void Telecom::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    if (new_cmd_received) {
        new_cmd_received = false;
        Data::get_instance().write(Data::EVENT_CMD_RECEIVED, &new_cmd_received);
        gpioWrite(LED_LORA_RX, 0);
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

    send_telemetry();

    // Write incoming packets to buffer
    // TODO: see if this can be called in handle_uplink during callback instead of polling
    update();
}

bool Telecom::begin() {
    lora_uplink.setPins(LORA_UPLINK_CS, LORA_UPLINK_RST, LORA_UPLINK_DI0);
    if (!lora_uplink.begin(UPLINK_FREQUENCY, SPI0)) {
        throw TelecomException("LoRa uplink init failed. Aborting LoRa downlink init.");
        return false;
    }else {
        Logger::log_eventf("LoRa uplink init succeeded!");
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
    /*
#if (UPLINK_INVERSE_IQ)
    lora_uplink.enableInvertIQ();
#else
    lora_uplink.disableInvertIQ();
#endif
*/

    // Set uplink radio as a continuous receiver
    lora_uplink.onReceive(handle_uplink);
    lora_uplink.receive();

    lora_downlink.setPins(LORA_DOWNLINK_CS, LORA_DOWNLINK_RST, LORA_DOWNLINK_DI0);
    if (!lora_downlink.begin(AV_DOWNLINK_FREQUENCY, SPI1)) {
        throw TelecomException("LoRa downlink init failed");
        return false;
    }else {
        Logger::log_eventf("LoRa downlink init succeeded!");
    }

    lora_downlink.setTxPower(AV_DOWNLINK_POWER);
    lora_downlink.setSignalBandwidth(AV_DOWNLINK_BW);
    lora_downlink.setSpreadingFactor(AV_DOWNLINK_SF);
    lora_downlink.setCodingRate4(AV_DOWNLINK_CR);
    lora_downlink.setPreambleLength(AV_DOWNLINK_PREAMBLE_LEN);

#if (DOWNLINK_CRC)
    lora_downlink.enableCrc();
#else
    lora_downlink.disableCrc();
#endif
    /*
#if (DOWNLINK_INVERSE_IQ)
    lora_downlink.enableInvertIQ();
#else
    lora_downlink.disableInvertIQ();
#endif
*/

    // Set downlink tx done callback
    lora_downlink.onTxDone(handle_tx_done);

    return true;
}

void Telecom::send_telemetry() {
    const DataDump data = Data::get_instance().get();

    av_downlink_unpacked_t packet;
    packet.packet_nbr = packet_number;
    packet.gnss_lon = data.nav.position.lng;
    packet.gnss_lat = data.nav.position.lat;
    packet.gnss_alt = data.nav.position.alt;
    packet.gnss_vertical_speed = data.nav.speed.norm();
    
    packet.N2_pressure = data.prop.N2_pressure;
    packet.N2_temp = data.prop.N2_temperature;
    packet.fuel_pressure = data.prop.fuel_pressure;
    packet.LOX_pressure = data.prop.LOX_pressure;
    packet.LOX_temp = data.prop.LOX_temperature;
    packet.LOX_inj_pressure = data.prop.LOX_inj_pressure;
    packet.LOX_inj_temp = data.prop.LOX_inj_temperature;
    packet.fuel_inj_pressure = data.prop.fuel_inj_pressure;
    packet.chamber_pressure = data.prop.chamber_pressure;

    const Valves valves(data.valves);
    packet.engine_state = ((valves.valve_dpr_vent_copv ^ 0) * ENGINE_STATE_VENT_N2)
                        | ((valves.valve_dpr_vent_lox ^ 1) * ENGINE_STATE_VENT_LOX) 
                        | ((valves.valve_dpr_vent_fuel ^ 1) * ENGINE_STATE_VENT_FUEL)
                        | ((valves.valve_dpr_pressure_lox ^ 0) * ENGINE_STATE_P_LOX)
                        | ((valves.valve_dpr_pressure_fuel ^ 0)* ENGINE_STATE_P_FUEL)
                        | ((valves.valve_prb_main_lox ^ 0) * ENGINE_STATE_MAIN_LOX)
                        | ((valves.valve_prb_main_fuel ^ 0) * ENGINE_STATE_MAIN_FUEL);

    packet.lpb_voltage = data.bat.lpb_voltage;
    packet.lpb_current = data.bat.lpb_current;
    packet.hpb_voltage = data.bat.hpb_voltage;
    packet.hpb_current = data.bat.hpb_current;

    packet.av_fc_temp = data.av_fc_temp;
    packet.ambient_temp = data.av_amb_temp;

    packet.av_state = (uint8_t)data.av_state;

    packet.cam_rec = data.cams_recording.cam_sep * CAMERA_REC_PARACHUTE
                     | data.cams_recording.cam_up * CAMERA_REC_AIRFRAME_UP
                     | data.cams_recording.cam_down * CAMERA_REC_AIRFRAME_DN;
    
    av_downlink_t compressed_packet(encode_downlink(packet));

 //   Logger::log_eventf(Logger::DEBUG, "Sending packet on downlink: %x, %x", packet.packet_nbr,
  //          (uint32_t)compressed_packet.packet_nbr);
//    std::cout << "compressed.packet_nbr: " << compressed_packet.packet_nbr << "\n"; 

    if (send_packet(CAPSULE_ID::AV_TELEMETRY, (uint8_t*)&compressed_packet, av_downlink_size)) {
        ++packet_number;
    }
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
    }

    std::cerr << "packet received\n";

    for (int i(0); i < packet_size; ++i) {
        uplink_buffer.write(lora_uplink.read());
    }
}

void Telecom::handle_capsule_uplink(uint8_t packet_id, uint8_t* data_in, uint32_t len) {
    switch (packet_id) {
        case CAPSULE_ID::GSC_CMD:
            gpioWrite(LED_LORA_RX, 1);

            memcpy(&last_packet, data_in, len);
            new_cmd_received = true;

            Data::get_instance().write(Data::TLM_CMD_ID, &last_packet.order_id);
            Data::get_instance().write(Data::TLM_CMD_VALUE, &last_packet.order_value);
            Data::get_instance().write(Data::EVENT_CMD_RECEIVED, &new_cmd_received);

	    {
            	const int order_id((int)last_packet.order_id);
            	const int order_value((int)last_packet.order_value);
            	//std::cout << "Command received from GS!\n"
                //	      << "ID: " << last_packet.order_id << "\n"
                //      	<< "Value: " << last_packet.order_value << "\n\n";

            	Logger::log_eventf("Received command from GSC.\t\tID: %i; Value: %i\n", last_packet.order_id, last_packet.order_value);
	    }

            break;
	case CAPSULE_ID::AV_TELEMETRY:
	    av_downlink_t radio_packet;
	    av_downlink_unpacked_t packet;
	    memcpy(&radio_packet, data_in, len);
	    packet = decode_downlink(radio_packet);
	    
	    std::cout << "packet_number: "
		      << packet.packet_nbr << "\n";
	    std::cout << "gnss_lon: "
		    << packet.gnss_lon << "\n";
	    std::cout << "gnss_lat: "
		    << packet.gnss_lat << "\n";
	    std::cout << "gnss_alt: "
		    << packet.gnss_alt << "\n";
	    std::cout << "N2_pressure: "
		    << packet.N2_pressure << "\n";
	    std::cout << "fuel_pressure: "
		    << packet.fuel_pressure << "\n";
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

void Telecom::handle_tx_done() {
    tx_done = true;
    gpioWrite(LED_LORA_TX, 0);
}

bool Telecom::send_packet(uint8_t packet_id, uint8_t* data, uint32_t len) {
    gpioWrite(LED_LORA_TX, 1);

    uint8_t* coded_buffer(capsule_downlink.encode(packet_id, data, len));
    size_t length(capsule_downlink.getCodedLen(len));

    if (!lora_downlink.beginPacket()) {
        return false;
    }
    lora_downlink.write(coded_buffer, length);
    lora_downlink.endPacket(true);

    delete[] coded_buffer;

    gpioWrite(LED_LORA_TX, 0);
    
    return true;
}
