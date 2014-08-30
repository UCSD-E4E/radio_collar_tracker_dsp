
#ifndef serialGPS_H_
#define serialGPS_H_
#include <stdbool.h>
int init_serial();

int open_port(const char* port);

bool setup_port(int fd, int baud, int data_bits, int stop_bits, bool parity, bool hardware_control);

int serial_read(int fd);

int return_long();

int return_lati();

int return_alti();

void close_port(int fd);

#endif
