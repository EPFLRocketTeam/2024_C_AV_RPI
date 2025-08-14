/**
 * The purpose of this file is to test the communication between two LoRa modules.
 * Alpha sends a packet to Bravo every 2(+-1)s.
 * Bravo is set in continuous Rx mode to test the good execution of interrupts.
 */

#include <iostream>
#include <LoRa.h>
#include <pigpio.h>
#include "config.h"
#include <Protocol.h>

#define LORA_BRAVO_SS_PIN     LORA_UPLINK_CS
#define LORA_BRAVO_RST_PIN    LORA_UPLINK_RST
#define LORA_BRAVO_DIO0_PIN   LORA_UPLINK_DI0

#define LORA_ALPHA_SS_PIN     LORA_DOWNLINK_CS
#define LORA_ALPHA_RST_PIN    LORA_DOWNLINK_RST
#define LORA_ALPHA_DIO0_PIN   LORA_DOWNLINK_DI0

#define lora_bravo LoRa
LoRaClass lora_alpha;

constexpr unsigned long LORA_FREQUENCY(866.34e6);

void dump_registers();
void alpha_send(String outgoing, int& count);
void uplink_receive(int packet_size);

int main(void) {
    gpioInitialise();

    std::cout << "\x1b[7m***RFM95 LoRa testbench***\x1b[0m\n\n";

    lora_alpha.setPins(LORA_ALPHA_SS_PIN, LORA_ALPHA_RST_PIN, LORA_ALPHA_DIO0_PIN);
    if (!lora_alpha.begin(LORA_FREQUENCY, SPI1)) {
        std::cout << "LoRa Alpha init failed!\n";
        return 1;
    }else {
        std::cout << "LoRa Alpha init succeeded!\n";
    }

    lora_alpha.setTxPower(AV_DOWNLINK_POWER);
    lora_alpha.setSignalBandwidth(AV_DOWNLINK_BW);
    lora_alpha.setSpreadingFactor(AV_DOWNLINK_SF);
    lora_alpha.setCodingRate4(AV_DOWNLINK_CR);
    lora_alpha.setPreambleLength(AV_DOWNLINK_PREAMBLE_LEN);

#if (DOWNLINK_CRC)
    lora_alpha.enableCrc();
#else
    lora_alpha.disableCrc();
#endif
#if (DOWNLINK_INVERSE_IQ)
//    lora_alpha.enableInvertIQ();
#else
 //   lora_alpha.disableInvertIQ();
#endif


    lora_bravo.setPins(LORA_BRAVO_SS_PIN, LORA_BRAVO_RST_PIN, LORA_BRAVO_DIO0_PIN);
    if (!lora_bravo.begin(LORA_FREQUENCY, SPI0)) {
        std::cout << "LoRa Bravo init failed!\n";
    }else {
        std::cout << "LoRa Bravo init succeeded!\n";
    }

    lora_bravo.setTxPower(UPLINK_POWER);
    lora_bravo.setSignalBandwidth(UPLINK_BW);
    lora_bravo.setSpreadingFactor(UPLINK_SF);
    lora_bravo.setCodingRate4(UPLINK_CR);
    lora_bravo.setPreambleLength(UPLINK_PREAMBLE_LEN);

#if (UPLINK_CRC)
    lora_bravo.enableCrc();
#else
    lora_bravo.disableCrc();
#endif
#if (UPLINK_INVERSE_IQ)
//    lora_bravo.enableInvertIQ();
#else
//    lora_bravo.disableInvertIQ();
#endif

    lora_bravo.receive();
    lora_bravo.onReceive(uplink_receive);

    //dump_registers();

    unsigned long last_send_time(0);
    int count(0);
    unsigned interval(2000);
    
    while (1) {
        if (gpioTick() / 1000 - last_send_time > interval) {
            String message("Alpha to Bravo");
            alpha_send(message, count);
            std::cout << std::dec << "\x1b[35m" "[" << count << "] Sending \"" << message << "\"\x1b[0m\n";
            last_send_time = gpioTick() / 1000;
            interval = (rand() % 2000) + 1000;
        }
    }
}

void dump_registers() {
    std::cout << "\nDumping registers...\n"
                 "\x1b[34m" "LoRa Alpha:\x1b[0m\n";
    lora_alpha.dumpRegisters(std::cout);
    std::cout << "\n\x1b[35m" "LoRa Bravo:\x1b[0m\n";
    lora_bravo.dumpRegisters(std::cout);
    std::cout << "\n";
}

void alpha_send(String outgoing, int& count) {
  lora_alpha.beginPacket();                   // start packet
  lora_alpha.write(count);                 // add message ID
  lora_alpha.write(outgoing.length());        // add payload length
  lora_alpha.print(outgoing);                 // add payload
  lora_alpha.endPacket(true);                     // finish packet and send it
  ++count;                           // increment message ID
}

void uplink_receive(int packet_size) {
    if (packet_size == 0) {
        return;
    }

    // read packet header bytes:
    uint8_t incomingMsgId = lora_bravo.read();     // incoming msg ID
    uint8_t incomingLength = lora_bravo.read();    // incoming msg length

    String incoming = "";

    while (lora_bravo.available()) {
        incoming += (char)lora_bravo.read();
    }

    if (incomingLength != incoming.length()) {   // check length for error
        printf("error: message length does not match length\n");
        return;                             // skip rest of function
    }

    // if message is for this device, or broadcast, print details:
    std::cout << "\x1b[93m" "Packet received" "\x1b[34m"
              << "\nMessage ID: " << String(incomingMsgId)
              << "\nMessage length: " << String(incomingLength)
              << "\nMessage: " << incoming
              << "\nRSSI: " <<  String(lora_bravo.packetRssi())
              << "\nSnr: " << String(lora_bravo.packetSnr())
              << "\x1b[0m\n\n";
}
