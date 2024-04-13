#include <iostream>
#include <LoRa.h>
#include <pigpio.h>

#define UPLINK_SS_PIN   LORA_DEFAULT_SPI_CE0
#define UPLINK_RST_PIN  LORA_DEFAULT_RESET_PIN
#define UPLINK_DIO0_PIN LORA_DEFAULT_DIO0_PIN

#define GROUND_STATION_SS_PIN     LORA_DEFAULT_SPI_CE1
#define GROUND_STATION_RST_PIN    24
#define GROUND_STATION_DIO0_PIN   6

#define lora_uplink LoRa
LoRaClass lora_ground;

constexpr unsigned long LORA_FREQUENCY(868e6);
constexpr uint8_t GS_ADDRESS(0xBB);
constexpr uint8_t UPLINK_ADDRESS(0xFF);

void dump_registers();
void ground_send(String outgoing, int& count);
void uplink_receive(int packet_size);

int main(void) {
    gpioInitialise();

    std::cout << "***RFM95 LoRa testbench***\n\n";

    lora_uplink.setPins(UPLINK_SS_PIN, UPLINK_RST_PIN, UPLINK_DIO0_PIN);
    if (!lora_uplink.begin(LORA_FREQUENCY, SPI0)) {
        std::cout << "LoRa uplink init failed!\n";
    }else {
        std::cout << "LoRa uplink init succeeded!\n";
    }

    lora_ground.setPins(GROUND_STATION_SS_PIN, GROUND_STATION_RST_PIN, GROUND_STATION_DIO0_PIN);
    if (!lora_ground.begin(LORA_FREQUENCY, SPI1)) {
        std::cout << "LoRa GS init failed!\n";
    }else {
        std::cout << "LoRa GS init succeeded!\n";
    }

    dump_registers();

    unsigned long last_send_time(0);
    int count(0);
    unsigned interval(2000);
    while (1) {
        if (gpioTick() / 1000 - last_send_time > interval) {
            String message("Command from GS to AV");
            ground_send(message, count);
            std::cout << std::dec << "\x1b[35m" "[" << count << "] Sending \"" << message << "\"\x1b[0m\n";
            last_send_time = gpioTick() / 1000;
            interval = (rand() % 2000) + 1000;
        }

        uplink_receive(lora_uplink.parsePacket());
    }
}

void dump_registers() {
    std::cout << "\nDumping registers...\n"
                 "\x1b[34m" "LoRa uplink:\x1b[0m\n";
    lora_uplink.dumpRegisters(std::cout);
    std::cout << "\n\x1b[35m" "LoRa GS:\x1b[0m\n";
    lora_ground.dumpRegisters(std::cout);
}

void ground_send(String outgoing, int& count) {
  lora_ground.beginPacket();                   // start packet
  lora_ground.write(UPLINK_ADDRESS);              // add destination address
  lora_ground.write(GS_ADDRESS);             // add sender address
  lora_ground.write(count);                 // add message ID
  lora_ground.write(outgoing.length());        // add payload length
  lora_ground.print(outgoing);                 // add payload
  lora_ground.endPacket();                     // finish packet and send it
  ++count;                           // increment message ID
}

void uplink_receive(int packet_size) {
    if (packet_size == 0) {
        return;
    }

    // read packet header bytes:
    int recipient = lora_uplink.read();          // recipient address
    uint8_t sender = lora_uplink.read();            // sender address
    uint8_t incomingMsgId = lora_uplink.read();     // incoming msg ID
    uint8_t incomingLength = lora_uplink.read();    // incoming msg length

    String incoming = "";

    while (lora_uplink.available()) {
        incoming += (char)lora_uplink.read();
    }

    if (incomingLength != incoming.length()) {   // check length for error
        printf("error: message length does not match length\n");
        return;                             // skip rest of function
    }

    // if the recipient isn't this device or broadcast,
    if (recipient != UPLINK_ADDRESS && recipient != 0xFF) {
        printf("This message is not for me.\n");
        return;                             // skip rest of function
    }

    // if message is for this device, or broadcast, print details:
    std::cout << "\x1b[34m";
    std::cout << "\nReceived from: 0x" <<  String(sender, HEX)
              << "\nSent to: 0x" << String(recipient, HEX)
              << "\nMessage ID: " << String(incomingMsgId)
              << "\nMessage length: " << String(incomingLength)
              << "\nMessage: " << incoming
              << "\nRSSI: " <<  String(lora_uplink.packetRssi())
              << "\nSnr: " << String(lora_uplink.packetSnr())
              << "\x1b[0m\n\n";
}