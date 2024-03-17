#include "telecom.h"

Telecom::Telecom() : new_cmd_received(false), last_packet{0, 0} {}

Telecom::~Telecom() {}

void Telecom::update() {
    /* TODO */
}

UPLink Telecom::get_cmd() const {
    UPLink out;
    out.id = static_cast<CMD_ID>(last_packet.order_id);
    out.value = last_packet.order_value;
    return out;
}

void Telecom::reset_cmd() {
    last_packet = {0, 0};
}