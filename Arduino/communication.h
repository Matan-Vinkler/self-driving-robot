#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <SPI.h>

#define START_BYTE 0x42

#define OPCODE_SET_SPEED_RIGHT 0x01
#define OPCODE_SET_SPEED_LEFT 0x02
#define OPCODE_FORWARD_RIGHT 0x03
#define OPCODE_FORWARD_LEFT 0x04
#define OPCODE_BACKWARD_RIGHT 0x05
#define OPCODE_BACKWARD_LEFT 0x06
#define OPCODE_OFF_RIGHT 0x07
#define OPCODE_OFF_LEFT 0x08

#define OPCODE_SET_SPEED_RIGHT_LEN 1
#define OPCODE_SET_SPEED_LEFT_LEN 1
#define OPCODE_FORWARD_RIGHT_LEN 0
#define OPCODE_FORWARD_LEFT_LEN 0
#define OPCODE_BACKWARD_RIGHT_LEN 0
#define OPCODE_BACKWARD_LEFT_LEN 0
#define OPCODE_OFF_RIGHT_LEN 0
#define OPCODE_OFF_LEFT_LEN 0

#define MAX_OPCODE 16

struct Command {
  byte opcode;
  size_t params_len;
  byte params[255];
};

bool start_command_red = false;
bool opcode_red = false;
int params_red = 0;
bool checksum_flag = false;

byte incomingByte = 0;
bool received = false;

byte checksum = 0;

void init_communication() {
  SPCR |= _BV(SPE);
  pinMode(MISO, OUTPUT);
  SPI.attachInterrupt();
}

ISR(SPI_STC_vect) {
  /* SPDR = data register */
  incomingByte = SPDR;
  received = true;
}

Command current_command = { 0 };

byte opcode_to_length(byte opcode) {
  switch (opcode) {
    case OPCODE_SET_SPEED_RIGHT:
      return OPCODE_SET_SPEED_RIGHT_LEN;
    case OPCODE_SET_SPEED_LEFT:
      return OPCODE_SET_SPEED_LEFT_LEN;
    case OPCODE_FORWARD_RIGHT:
      return OPCODE_FORWARD_RIGHT_LEN;
    case OPCODE_FORWARD_LEFT:
      return OPCODE_FORWARD_LEFT_LEN;
    case OPCODE_BACKWARD_RIGHT:
      return OPCODE_BACKWARD_RIGHT_LEN;
    case OPCODE_BACKWARD_LEFT:
      return OPCODE_BACKWARD_LEFT_LEN;
    case OPCODE_OFF_RIGHT:
      return OPCODE_OFF_RIGHT_LEN;
    case OPCODE_OFF_LEFT:
      return OPCODE_OFF_LEFT_LEN;
    default:
      return 0;
  }
}

Command* read_command() {
  Command* command = NULL;
  if (received) {
    received = false;
    SPDR = 0;

    if (!start_command_red) {
      start_command_red = (incomingByte == START_BYTE);
      checksum ^= incomingByte;
    }

    else if (!opcode_red) {
      current_command.opcode = incomingByte;
      current_command.params_len = opcode_to_length(current_command.opcode);
      opcode_red = true;

      if (current_command.params_len == 0) {
        checksum_flag = true;
      }

      checksum ^= incomingByte;
    }

    else if (!checksum_flag) {
      current_command.params[params_red] = incomingByte;
      params_red++;
      if (params_red == current_command.params_len) {
        checksum_flag = true;
      }

      checksum ^= incomingByte;
    }

    else {
      if (incomingByte == checksum) {
        command = &current_command;
      }
    }
  }

  return command;
}

void reset_command() {
  current_command = { 0 };
}

void reset_flags() {
  start_command_red = false;
  opcode_red = false;
  params_red = 0;
  checksum = 0;
  checksum_flag = false;
}

#endif