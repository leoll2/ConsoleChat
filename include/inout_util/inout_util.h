#ifndef INOUT_UTIL_H_
#define INOUT_UTIL_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void clear_escape();
uint16_t count_columns(char* buf, uint16_t pos);
void flush_input();
void read_stdin(char* buf, int* pos, int max_len, char* user);
void read_msg_stdin(char* buf, uint16_t* pos, uint16_t max_len);
void reset_line(uint16_t columns);

#endif
