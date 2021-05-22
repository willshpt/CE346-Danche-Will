// LED Matrix app
//
// Display messages on the LED matrix

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"

#include "led_matrix.h"
#include "microbit_v2.h"


int main(void) {
  printf("Board started!\n");
  
  // initialize LED matrix driver
  led_matrix_init();

  // call other functions here

  // loop forever
  char *strone = "Hi CE346!";
  char *strtwo = "It works!";
  while (1) {
    print_string(strone, 0.5);
    nrf_delay_ms(3000);
    print_string(strtwo, 0.3);
    nrf_delay_ms(3000);

  }
}

