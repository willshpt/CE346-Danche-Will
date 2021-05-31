#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Initialize the LED matrix display
void led_matrix_init(void);
void update_matrix(uint8_t*);
// Print a string on repeat
void init_print_string(char* str, double letter_delay);
// Change the string value
void new_print_string(char* str);

