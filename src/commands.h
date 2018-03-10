#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <string.h>
#include "chat.h"

extern command commands[NUM_OF_COMMANDS];

int exec_comm(int sock, uint16_t cmd_code, char* user);
uint16_t get_cmd_code(char* cmd);
int get_num_args(uint16_t cmd_code);
int set_handler(uint16_t cmd_code, generic_fp fun);
void show_commands();

#endif
