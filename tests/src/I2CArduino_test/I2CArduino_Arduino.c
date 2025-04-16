/**********************************************/
/* I2C Interface Arduino Test                 */
/*                                            */
/* Arduino File                               */
/*                                            */
/* THIS FILE SHOULD NEVER BE COMPILED WITH    */
/* 2024_C_AV_RPI AND SHOULD BE FLASHED WITH   */
/* ARDUINO IDE                                */
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

#include "Wire.h"

// Message structure, we assume that it will never go over 32 bytes
struct Message {
  char message[32];
  int size;
};

// Next message to be sent on request
Message msg_to_send;

// Receive and Send constants
const int   hello_world_size     = 15;
const char* hello_world_expected = "Hello, World !";
const char* hello_rpi_send_back  = "Hello, RPI !";

// Read everything into a message
Message read_all () {
  Message msg;
  msg.size = 0;
  while (Wire.available())
    msg.message[msg.size ++] = Wire.read();
  return msg;
}

// Echo Op Receive
void Handle_OnReceive_Echo () {
  msg_to_send = read_all();
}
// Hello World Receive
void Handle_OnReceive_HelloWorld () {
  Message msg = read_all();

  // Check it is valid
  if (msg.size != hello_world_size) return ;
  for (int i = 0; i < msg.size; i ++) {
    if (msg.message[i] != hello_world_expected[i]) {
      return ;
    }
  }

  // Send the reply
  Message back;
  back.size = 14;
  for (int i = 0; i < back.size; i ++)
    back.message[i] = hello_rpi_send_back[i];
  msg_to_send = back;
}
// Send Random Message
void Handle_OnReceive_SendRandom () {
  msg_to_send.size = 1;
  msg_to_send.message[0] = random(166);
}

// Handle receive and switch to corresponding receive function
void Handle_OnReceive (int numBytes) {
  if (!Wire.available()) return ;

  int op_code = Wire.read();
  
  switch (op_code) {
    case 0:
      Handle_OnReceive_Echo ();
      break ;
    case 1:
      Handle_OnReceive_HelloWorld();
      break ;
    case 2:
      Handle_OnReceive_SendRandom();
      break ;
    default:
      break ;
  }

  // WARNING to modify depending on the board, can crash if too low
  delayMicroseconds(350);
}

// On request, send the current message if it exists
void Handle_OnRequest () {
  if (msg_to_send.size == 0) {
    return ;
  }

  Wire.write(msg_to_send.message, msg_to_send.size);
  msg_to_send.size = 0;
}

// Setup I2C and setup no message
void setup() {
  Serial.begin(9600);

  msg_to_send.size = 0;

  Wire.begin(0x42);
  Wire.onReceive(Handle_OnReceive);
  Wire.onRequest(Handle_OnRequest);
}

// Nothing happens in the loop
void loop() {}
