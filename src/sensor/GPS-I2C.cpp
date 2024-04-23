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

#include "GPS-I2C.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

//Sets up the sensor for constant read
//Throws exception if unable to communicate with sensor
I2CGPS::I2CGPS(uint8_t addr) : addr(addr) {
  try {
    I2CInterface::getInstance().open(addr);
  } catch(const I2CInterfaceException& e) {
    throw I2CGPSException(I2CGPS_E_COMM_FAIL);
  }

  _head = 0; //Reset the location holder
  _tail = 0;
}

I2CGPS::~I2CGPS(){
  try {
    I2CInterface::getInstance().close(addr);
  } catch(const I2CInterfaceException& e) {
    throw I2CGPSException(I2CGPS_E_COMM_FAIL);
  }

  _head = 0; //Reset the location holder
  _tail = 0;
}

//Polls the GPS module to see if new data is available
//Reads a 255 byte packet from GPS module
//If new data is there, appends it to the gpsData array
void I2CGPS::check() {
  uint8_t tmp[MAX_PACKET_SIZE] = {0};
  try {
    I2CInterface::getInstance().read(addr, 0x00, tmp, MAX_PACKET_SIZE);
  } catch(const I2CInterfaceException& e) {
    throw I2CGPSException(I2CGPS_E_COMM_FAIL);
  }

  for (uint8_t i = 0; i < MAX_PACKET_SIZE; i++)
  {
    if (tmp[i] != 0x0A)
    {
      //Record this byte
      gpsData[_head++] = tmp[i];
      _head %= MAX_PACKET_SIZE; //Wrap variable

      if (_head == _tail) {
        _tail++; //If we're overwriting the tail, move it up
        throw I2CGPSException(I2CGPS_E_OVERRUN);
      }
    }
  }
}

//Returns # of available bytes that can be read
uint8_t I2CGPS::available() {
  //If tail=head then no new data is available in the local buffer
  //So now check to see if the module has anything new in its buffer
  if (_tail == _head)
  {
    check(); //Check to module to see if new I2C bytes are available
  }

  //Return new data count
  if (_head > _tail)
    return (_head - _tail);
  if (_tail > _head)
    return (MAX_PACKET_SIZE - _tail + _head);
  return (0); //No data available
}

//Returns the next available byte from the gpsData array
//Returns 0 if no byte available
uint8_t I2CGPS::read() {
  if (_tail != _head)
  {
    uint8_t datum = gpsData[_tail++];
    if (_tail == MAX_PACKET_SIZE)
      _tail = 0; //Wrap variable
    return (datum);
  }
  else
    return (0); //No new data
}


//Functions for sending commands to the GPS module
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Send a given command or configuration string to the module
//The input buffer on the MTK is 255 bytes. Caller must keep strings shorter than 255 bytes
//Any time you end transmission you must give the module 10ms to process bytes
void I2CGPS::sendMTKpacket(std::string command) {
  if (command.length() > 255) {
    std::cout << "Command too long" << std::endl;

    return;
  }
  try {
    I2CInterface::getInstance().write(addr, 0x00, (uint8_t*)command.c_str(), command.length());
  } catch(const I2CInterfaceException& e) {
    throw I2CGPSException(I2CGPS_E_COMM_FAIL);
  }
  usleep(10000); //Give the module 10ms to process bytes
}


//Given a packetType and any settings, return string that is a full
//config sentence complete with CRC and \r \n ending bytes
//PMTK uses a different packet numbers to do configure the module.
//These vary from 0 to 999. See 'MTK NMEA Packet' datasheet for more info.
std::string I2CGPS::createMTKpacket(uint16_t packetType, std::string dataField)
{
  //Build config sentence using packetType
  std::string configSentence = "";
  configSentence += "$PMTK"; //Default header for all GPS config messages

  //Attach the packetType number
  //Append any leading zeros
  if (packetType < 100)
    configSentence += "0";
  if (packetType < 10)
    configSentence += "0";
  configSentence += std::to_string(packetType);

  //Attach any settings
  if (dataField.length() > 0)
    configSentence += dataField; //Attach the string of flags

  configSentence += "*"; //Attach end tag

  configSentence += calcCRCforMTK(configSentence); //Attach CRC

  //Attach ending bytes
  configSentence += '\r'; //Carriage return
  configSentence += '\n'; //Line feed

  return (configSentence);
}

//Calculate CRC for MTK messages
//Given a string of characters, XOR them all together and return CRC in string form
std::string I2CGPS::calcCRCforMTK(std::string sentence)
{
  uint8_t crc = 0;

  //We need to ignore the first character $
  //And the last character *
  for (uint8_t x = 1; x < sentence.length() - 1; x++)
    crc ^= sentence[x]; //XOR this byte with all the others

  std::string output = "";
  if (crc < 10)
    output += "0"; //Append leading zero if needed

  //Convert the CRC to a string
  std::stringstream ss;
  ss << std::uppercase << std::hex << static_cast<int>(crc);
  output += ss.str();

  return (output);
}

void I2CGPS::sendPGCMDpacket(std::string command) {
  sendMTKpacket(command); // Send process is the same, re-named to ease user's minds
}

std::string I2CGPS::createPGCMDpacket(uint16_t packetType, std::string dataField){
  //Build config sentence using packetType
  std::string configSentence = "";
  configSentence += "$PGCMD,"; //Default header for all PGCMD messages

  //Attach the packetType number
  //Append any leading zeros
  if (packetType < 100)
    configSentence += "0";
  if (packetType < 10)
    configSentence += "0";
  configSentence += std::to_string(packetType);

  //Attach any settings
  if (dataField.length() > 0)
    configSentence += dataField; //Attach the string of flags

  configSentence += "*"; //Attach end tag

  configSentence += calcCRCforMTK(configSentence); //Attach CRC - uses the same crc as PTMK

  //Attach ending bytes
  configSentence += '\r'; //Carriage return
  configSentence += '\n'; //Line feed

  return (configSentence);
}
