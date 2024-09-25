/**********************************************/
/* I2C Interface Arduino Test                 */
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
/*                                            */
/* The arduino code works in the following    */
/* way depending on the first byte :          */
/*  - case 0 : echo the next byte             */
/*  - case 1 : expect "Hello, World !" to be  */
/*             received                       */
/**********************************************/

#include "I2CInterface.h"

#include <sstream>
#include <iostream>
#include <optional>
#include <string>
#include <stdlib.h>

/**
 * On start of action
 * 
 * Print the action followed by 3 dots
 */
void on_action (const char* action) {
    std::cout << action << "... ";
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
    std::cout << "\\033[1m\\033[91mERROR\\033[0m\n";
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
    std::cout << "\\033[1m\\033[91mWRONG DATA\\033[0m\n";
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
    std::cout << "\\033[1m\\033[92mDONE\\033[0m\n";
}

const int ARDUINO_ADDRESS = 0x42;

uint8_t hello_world[16] = { 1, 72, 101, 108, 108, 111, 44, 32, 87, 111, 114, 108, 100, 32, 33, 0 };

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
    } catch (I2CInterfaceException exception) {
        return on_action_error(exception);
    }
}
