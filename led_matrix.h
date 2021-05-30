#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "virtual_timer.h"

// Initialize the LED matrix display
void led_matrix_init(void);

extern char string[6];

// You may need to add more functions here
void led_char(char);
//void led_str(char*);
void led_str(void);

