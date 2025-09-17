#ifndef HANDLE_COMMAND_H_
#define HANDLE_COMMAND_H_

#include "communication.h"
#include "motors.h"

void handle_command(Command* cmd) {
  if(cmd->opcode == OPCODE_SET_SPEED_RIGHT) {
    setspeed_motor_a(cmd->params[0]);
  }

  if(cmd->opcode == OPCODE_SET_SPEED_LEFT) {
    setspeed_motor_b(cmd->params[0]);
  }

  if(cmd->opcode == OPCODE_FORWARD_RIGHT) {
    turn_forward_motor_a();
  }

  if(cmd->opcode == OPCODE_FORWARD_LEFT) {
    turn_forward_motor_b();
  }

  if(cmd->opcode == OPCODE_BACKWARD_RIGHT) {
    turn_backward_motor_a();
  }

  if(cmd->opcode == OPCODE_BACKWARD_LEFT) {
    turn_backward_motor_b();
  }

  if(cmd->opcode == OPCODE_OFF_RIGHT) {
    turn_off_motor_a();
  }

  if(cmd->opcode == OPCODE_OFF_LEFT) {
    turn_off_motor_b();
  }
}

#endif