#ifndef CAPSULE_H
#define CAPSULE_H

#define MAX_BUFFER_SIZE 1024
#define ADDITIONAL_BYTES 5

#define PRA_DEFAULT 0xFF
#define PRB_DEFAULT 0xFA

#include <cstdint> // need to add this for C++ Ubuntu for uint8_t

enum ParserState {
    PREAMBLE_A = 0,
    PREAMBLE_B,
    PACKET_ID,
    LENGTH,
    PAYLOAD,
    CRC
};

template<typename T>
class Capsule {
  public:
    Capsule(void (*function)(uint8_t, uint8_t*, uint32_t), uint8_t PRAIn = PRA_DEFAULT, uint8_t PRBIn = PRB_DEFAULT):
            PRA(PRAIn), 
            PRB(PRBIn), 
            currentState(PREAMBLE_A), 
            lenCount(0), 
            functionCallBack(function), 
            classPtr(nullptr) 
    {}
    Capsule(void (T::*function)(uint8_t, uint8_t*, uint32_t), T* tptrIn, uint8_t PRAIn = PRA_DEFAULT, uint8_t PRBIn = PRB_DEFAULT): 
            PRA(PRAIn), 
            PRB(PRBIn), 
            currentState(PREAMBLE_A), 
            lenCount(0), 
            functionCallBackClass(function), 
            classPtr(tptrIn) 
    {}
    uint32_t getCodedLen(uint32_t lenIn) { return lenIn + ADDITIONAL_BYTES; }
    void decode(uint8_t dataIn) {
      switch (currentState) {
        case PREAMBLE_A:
            //Serial.println("PREAMBLE_A");
            if (dataIn == PRA) {
                currentState = PREAMBLE_B;
            }
        break;
        case PREAMBLE_B:
            //Serial.println("PREAMBLE_B");
            if (dataIn == PRB) {
                currentState = PACKET_ID;
                packetId = 0x00;
                for (uint32_t i(0); i < MAX_BUFFER_SIZE; i++) {
                    buffer[i] = 0;
                }
            } else {
                currentState = PREAMBLE_A;
            }
        break;
        case PACKET_ID:
            //Serial.println("PACKET_ID");
            packetId = dataIn;
            currentState = LENGTH;
        break;
        case LENGTH:
            //Serial.println("LENGTH");
            len = dataIn;
            lenCount = 0;
            currentState = PAYLOAD;
        break;
        case PAYLOAD:
            //Serial.println("PAYLOAD");
            if (lenCount < len) {
                buffer[lenCount] = dataIn;
                lenCount++;
                if (lenCount == len) {
                    currentState = CRC;
                }
            }
            else {
                currentState = CRC;
            }
        break;
        case CRC:
            //Serial.println("CRC");
            uint8_t checkSum = 0;
            for (uint32_t i(0); i < len; i++) {
                checkSum += buffer[i];
            }
            if (checkSum == dataIn) {
                //Serial.println("Calling function");
                if (classPtr) {
                    (classPtr->*functionCallBackClass)(packetId, buffer, len);
                }
                else {
                    functionCallBack(packetId, buffer, len);
                } 
            }
            else {
                // Should return error code maybe?
            }
            currentState = PREAMBLE_A;
        break;
      }
    }
    uint8_t* encode(uint8_t packetId, uint8_t *packetIn, uint32_t lenIn) {
      uint32_t lenOut = getCodedLen(lenIn);
      uint8_t* packetOut = new uint8_t[lenOut];

      packetOut[0] = PRA;
      packetOut[1] = PRB;
      packetOut[2] = packetId;
      packetOut[3] = lenIn;

      uint8_t checkSum = 0;
      for (uint32_t i(0); i < lenIn; i++) {
          packetOut[i + 4] = packetIn[i];
          checkSum += packetIn[i];
      }
      packetOut[lenIn + 4] = checkSum; 
      return packetOut;
    }
  private:
    uint8_t buffer[MAX_BUFFER_SIZE];
    uint8_t PRA;
    uint8_t PRB;
    uint8_t packetId;
    uint8_t len;
    ParserState currentState;
    uint32_t lenCount;
    void (*functionCallBack)(uint8_t packetId, uint8_t* dataIn, uint32_t len);
    void (T::*functionCallBackClass)(uint8_t packetId, uint8_t* dataIn, uint32_t len);
    T* classPtr;
};

// This is used to avoid having to add <> when using the class at global level
class Foo {};
using CapsuleStatic = Capsule<Foo>;

#endif    // end CAPSULE_H