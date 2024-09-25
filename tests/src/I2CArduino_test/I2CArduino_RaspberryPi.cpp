/**********************************************/
/* I2C Interface Arduino Test                 */
/*                                            */
/* Raspberry Pi File                          */
/*                                            */
/* Test the following protocol                */
/*  - Init I2CInterface                       */
/*  - Open Handle 0x42                        */
/*  - Send 1 + "Hello, World !" to Arduino    */
/*  - Expect to receive "Hello, RPI !"        */
/*  - For every byte from 0 to 255            */
/*    in a random order                       */
/*    - Send 0 followed by the byte and wait  */
/*      For an echo                           */
/*  - Do 10_000 read on register 2            */
/*  - Check all received bytes are <= 165     */
/*  - Check that the entropy is around        */
/*    log(165)                                */
/*                                            */
/* The arduino code works in the following    */
/* way depending on the first byte :          */
/*  - case 0 : echo the next byte             */
/*  - case 1 : expect "Hello, World !" to be  */
/*             received                       */
/*  - case 2 : send a random byte <= 165      */
/*    (We will assume that if an error        */
/*    occurs, it will be unlikely for the     */
/*    both the entropy to go to log(166), and */
/*    all numbers to stay <= 165)             */
/*                                            */
/*                                            */
/* WARNING / TROUBLESHOOTING                  */
/*                                            */
/* In case you have an issue in the echo part */
/* or in the random part, modify the arduino  */
/* code to increase the delay as it might     */
/* be the cause of the crash, and not the     */
/* drivers fault.                             */
/*                                            */
/**********************************************/

#include "I2CInterface.h"

#include <sstream>
#include <iostream>
#include <optional>
#include <string>
#include <math.h>
#include <cstring>
#include <stdlib.h>

/**
 * On start of action
 * 
 * Print the action followed by 3 dots
 */
void on_action (const char* action) {
    std::cout << action << "... ";
    
    int length = strlen(action);
    int padLen = 30;
    while (length < padLen) {
        std::cout << " ";
        length ++;
    }
}
/**
 * On action error
 * 
 * @param exception the exception thrown
 * 
 * Displays ERROR in red using ANSI Escape characters
 * Displays the message of the error
 */
int on_action_error (I2CInterfaceException exception) {
    std::cout << "\033[1m\033[91mERROR\033[0m\n";
    std::cout << exception.what() << "\n";
    return 1;
}
/**
 * On action result
 * 
 * @param value_error_message the error message, if it exists
 * 
 * If there is no error, return otherwise
 * 
 * Displays WRONG DATA in red using ANSI Escape characters
 * Displays the message of the error
 * Exit with error code 1.
 */
int on_action_result (std::optional<std::string> value_error_message) {
    if (!value_error_message.has_value()) return 0;
    std::cout << "\033[1m\033[91mWRONG DATA\033[0m\n";
    std::cout << value_error_message.value() << "\n";

    exit(1);
    return 1;
}
/**
 * On action done
 * 
 * Displays DONE in green using ANSI Escape characters
 */
void on_action_done () {
    std::cout << "\033[1m\033[92mDONE\033[0m\n";
}

const int ARDUINO_ADDRESS = 0x42;

const int ECHO_OP = 0;
const int HELLO_OP = 1;
const int RANDOM_OP = 2;

const int RANDOM_ITERATION = 10000;

uint8_t hello_world[16] = { HELLO_OP, 72, 101, 108, 108, 111, 44, 32, 87, 111, 114, 108, 100, 32, 33, 0 };

const int hello_rpi_size = 13;
uint8_t hello_rpi_expected [hello_rpi_size] = { 72, 101, 108, 108, 111, 44, 32, 82, 80, 73, 32, 33, 0 };
uint8_t hello_rpi_buffer   [hello_rpi_size] = { 0 };

void put_into (std::stringstream& stream, uint8_t* data, int size) {
    stream << "[";

    bool first = true;
    for (int i = 0; i < size; i ++) {
        if (!first) stream << ", ";
        stream << data[i];

        first = false;
    }
    stream << "]";
}
std::optional<std::string> make_hello_data_error_message () {
    bool valid = true;
    for (int i = 0; i < hello_rpi_size; i ++)
        valid &= hello_rpi_expected[i] == hello_rpi_buffer[i];
    if (valid) return {};

    std::stringstream message_buffer;
    message_buffer << "Invalid Hello, RPI ! Message\n";
    message_buffer << " - Expected : ";
    put_into(message_buffer, hello_rpi_expected, hello_rpi_size);
    message_buffer << "\n";
    message_buffer << " - Received : ";
    put_into(message_buffer, hello_rpi_buffer, hello_rpi_size);
    message_buffer << "\n";
    message_buffer << "\n";
    message_buffer << "WARNING The arduino only sends the reply if the hello was correct, so the error might come from the send of \"Hello, World !\".";

    return message_buffer.str();
}

int main (void) {
    try {
        std::cout << "=== I2C Interface Test :: Arduino ===\n";

        on_action( "Initializing I2C Interface" );
        I2CInterface& i2c = I2CInterface::getInstance();
        on_action_done();

        on_action( "Openning I2C Device" );
        i2c.open( ARDUINO_ADDRESS );
        on_action_done();

        on_action( "Writing Hello, World !" );
        i2c.writeDevice( ARDUINO_ADDRESS, hello_world, sizeof(hello_world) );
        on_action_done();

        on_action( "Receive Hello, RPI !" );
        i2c.readDevice( ARDUINO_ADDRESS, hello_rpi_buffer, hello_rpi_size );
        on_action_result( make_hello_data_error_message() );
        on_action_done();

        on_action( "Echo 256 bytes" );
        for (int i = 0; i < 256; i ++) {
            int r = (57 * i + 154) & 0xFF;

            uint8_t data[1] = { r };
            i2c.write( ARDUINO_ADDRESS, ECHO_OP, data, 1 );
            i2c.readDevice( ARDUINO_ADDRESS, data, 1 );

            if (r != data[0]) {
                std::stringstream message_buffer;
                message_buffer << "Invalid Echo Result\n";
                message_buffer << " - Expected : " << (int) (r) << "\n";
                message_buffer << " - Received : " << (int) (data[0]);

                on_action_result(message_buffer.str());
            }
        }
        on_action_done();
    
        int amount[256];
        for (int i = 0; i < 256; i ++) amount[i] = 0;

        on_action("Querying Random Numbers");
        for (int i = 0; i < RANDOM_ITERATION; i ++) {
            uint8_t v[1] = { 0 };
            i2c.read( ARDUINO_ADDRESS, RANDOM_OP, v, 1 );

            amount[v[0]] ++;
            if (v[0] > 165) {
                std::stringstream message_buffer;
                message_buffer << "Invalid Random Result\n";
                message_buffer << " - Received : " << (int) (v[0]) << "\n";

                on_action_result(message_buffer.str());
            }
        }
        on_action_done();

        on_action("Computing Entropy of Result");
        float entropy = 0;
        for (int i = 0; i < 256; i ++) {
            if (amount[i] != 0) {
                float probability = amount[i] / RANDOM_ITERATION;

                entropy += - probability * log2(probability);
            }
        }

        float expected_entropy = log2(166);
        if (expected_entropy - entropy > 0.1) {
            std::stringstream message_buffer;
            message_buffer << "Invalid Entropy of random rumbers\n";
            message_buffer << " - Expected : " << expected_entropy << "\n";
            message_buffer << " - Received : " << entropy << "\n";

            on_action_result(message_buffer.str());
        }
        on_action_done();
    } catch (I2CInterfaceException exception) {
        return on_action_error(exception);
    }
}
