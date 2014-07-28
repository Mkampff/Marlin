#include "serial_number.h"

#include <string.h>

char serial_number[MAX_SERIAL_NUMBER_WIDTH];

void set_serial_number(const char* s)
{
    // copy data
    strncpy(serial_number, s, MAX_SERIAL_NUMBER_WIDTH);
    // truncate if a longer string was given
    serial_number[MAX_SERIAL_NUMBER_WIDTH-1] = '\0';
}

