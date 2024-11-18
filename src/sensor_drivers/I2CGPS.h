/*
  This is a library written for the MediaTek MT3333 based GPS module with I2C firmware
  specially loaded. SparkFun sells these at its website: www.sparkfun.com

  Written by Nathan Seidle @ SparkFun Electronics, April 25th, 2017

  This GPS module is special because it can use an I2C interface to communicate.

  This library handles the pulling in of data over I2C. We recommend a parsing engine
  such as TinyGPS.

  https://github.com/sparkfun/SparkFun_I2C_GPS_Arduino_Library

  Do you like this library? Help support SparkFun. Buy a board!

  Development environment specifics:
  Arduino IDE 1.8.1

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef I2CGPS_h
#define I2CGPS_h

#include <stdint.h>
#include <string>
#include "I2CGPS_interface.h"

/**\name API error codes */
#define I2CGPS_E_NULL_PTR                         INT8_C(-1)
#define I2CGPS_E_COMM_FAIL                        INT8_C(-2)
#define I2CGPS_E_OVERRUN                          INT8_C(-3)
#define I2CGPS_E_CMD_EXEC_FAILED                  INT8_C(-4)
#define I2CGPS_E_CONFIG_ERROR                     INT8_C(-5)
#define I2CGPS_E_INVALID_LEN                      INT8_C(-6)
#define I2CGPS_E_DEV_NOT_FOUND                    INT8_C(-7)

#define I2CGPS_ADDR 0x10 //7-bit unshifted default I2C Address

#define MAX_PACKET_SIZE 255
//If packet size is ever more than 255 the head and tail variables will need to be
//changed to something other than uint8_t

#define I2C_SPEED_STANDARD        100000
#define I2C_SPEED_FAST            400000

class I2CGPS : public I2CGPS_Interface{
private:
  uint8_t addr; //I2C address of the GPS module

  uint8_t _head; //Location of next available spot in the gpsData array. Limited to 255.
  uint8_t _tail; //Location of last spot read from gpsData array. Limited to 255.
  uint8_t gpsData[MAX_PACKET_SIZE] = {0}; //The place to store valid incoming gps data
public:
  I2CGPS(uint8_t addr = I2CGPS_ADDR);
  ~I2CGPS() override;

  void check() override; //Checks module for new data
  uint8_t available() override; //Returns available number of bytes. Will call check() if zero is available.
  uint8_t read() override; //Returns the next available byte

  void sendMTKpacket(std::string command) override;
  std::string createMTKpacket(uint16_t packetType, std::string dataField) override;
  std::string calcCRCforMTK(std::string sentence) override; //XORs all bytes between $ and *

  void sendPGCMDpacket(std::string command) override;
  std::string createPGCMDpacket(uint16_t packetType, std::string dataField) override;
};

class I2CGPSException : public std::exception {
private:
  std::string message;

public:
  I2CGPSException(int8_t errorCode) {
    switch (errorCode) {
      case I2CGPS_E_NULL_PTR:
        message = "Null pointer error.";
        break;
      case I2CGPS_E_COMM_FAIL:
        message = "Communication failure.";
        break;
      case I2CGPS_E_OVERRUN:
        message = "Buffer overrun";
        break;
      case I2CGPS_E_CMD_EXEC_FAILED:
        message = "Command execution failed.";
        break;
      case I2CGPS_E_CONFIG_ERROR:
        message = "Configuration error.";
        break;
      case I2CGPS_E_INVALID_LEN:
        message = "Invalid length error.";
        break;
      case I2CGPS_E_DEV_NOT_FOUND:
        message = "Device not found.";
        break;
      default:
        message = "Unknown error.";
        break;
    }
  }

  virtual const char* what() const throw () {
    return message.c_str();
  }
};

#endif //GPS_I2C_h
