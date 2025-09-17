#include "motors.h"
#include "communication.h"

#include "handle_command.h"

//TODO: Integrate between "communication.h" and "motors.h"

void setup() {
  init_communication();
  init_motors();
  reset_flags();
  Serial.begin(115200);

  Serial.println("Begin!");
}

void print_command(Command* cmd) {
  Serial.print("Opcode: ");
  Serial.println(cmd->opcode);

  Serial.print("Param len: ");
  Serial.println(cmd->params_len);

  Serial.print("Params: ");
  for (int i = 0; i < cmd->params_len; i++) {
    Serial.print(cmd->params[i]);
    Serial.print(" ");
  }

  Serial.println("\n");
}

void loop() {
  Command* cmd = read_command();
  if (cmd) {
    handle_command(cmd);
    //print_command(cmd);

    reset_flags();
    reset_command();
  }
}
