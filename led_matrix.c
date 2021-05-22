// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"


APP_TIMER_DEF(my_timer_1);
uint8_t led_states[5] = {0xff, 0xff, 0xff, 0xff, 0xff};
uint8_t current_row = 0;
char *string;
int current_char;

void print_string(char* str, double letter_delay){
  current_char = 0;
  if(str[current_char] != 0){
    string = str;
    // New timer
  }
}

void print_char(){
  if(string[current_char] != 0){
    update_matrix(font[string[current_char]]);
    current_char++;
  }
  else{
    // End timer
  }
}

void update_matrix(uint8_t thingmatrix[5]){
  led_states[0] = thingmatrix[0];
  led_states[1] = thingmatrix[1];
  led_states[2] = thingmatrix[2];
  led_states[3] = thingmatrix[3];
  led_states[4] = thingmatrix[4];
}

void update_leds(){
  switch(current_row) {
    case 0 :
      nrf_gpio_pin_clear(LED_ROW5);
      break;
    case 1 :
      nrf_gpio_pin_clear(LED_ROW1);
      break;
    case 2 :
      nrf_gpio_pin_clear(LED_ROW2);
      break;
    case 3 :
      nrf_gpio_pin_clear(LED_ROW3);
      break;
    case 4 :
      nrf_gpio_pin_clear(LED_ROW4);
      break;
  }
  nrf_gpio_pin_write(LED_COL1, !(led_states[current_row] & 0b00001));
  nrf_gpio_pin_write(LED_COL2, !(led_states[current_row] & 0b00010));
  nrf_gpio_pin_write(LED_COL3, !(led_states[current_row] & 0b00100));
  nrf_gpio_pin_write(LED_COL4, !(led_states[current_row] & 0b01000));
  nrf_gpio_pin_write(LED_COL5, !(led_states[current_row] & 0b10000));
  switch(current_row) {
    case 0 :
      nrf_gpio_pin_set(LED_ROW1);
      break;
    case 1 :
      nrf_gpio_pin_set(LED_ROW2);
      break;
    case 2 :
      nrf_gpio_pin_set(LED_ROW3);
      break;
    case 3 :
      nrf_gpio_pin_set(LED_ROW4);
      break;
    case 4 :
      nrf_gpio_pin_set(LED_ROW5);
      break;
  }
  if(++current_row > 4){
    current_row = 0;
  }
}

void led_matrix_init(void) {
  // initialize row pins
  nrf_gpio_pin_dir_set(LED_ROW1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW4, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW5, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_clear(LED_ROW1);
  nrf_gpio_pin_clear(LED_ROW2);
  nrf_gpio_pin_clear(LED_ROW3);
  nrf_gpio_pin_clear(LED_ROW4);
  nrf_gpio_pin_clear(LED_ROW5);

  // initialize col pins
  nrf_gpio_pin_dir_set(LED_COL1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL4, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL5, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_clear(LED_COL1);
  nrf_gpio_pin_clear(LED_COL2);
  nrf_gpio_pin_clear(LED_COL3);
  nrf_gpio_pin_clear(LED_COL4);
  nrf_gpio_pin_clear(LED_COL5);

  // set default values for pins
  // To activate an LED, ensure that its corresponding row 
  // pin is high and its corresponding column pin is low
  //nrf_gpio_pin_set(LED_ROW1);
  //nrf_gpio_pin_set(LED_COL3);

  // initialize timer(s) (Part 3 and onwards)
  app_timer_init();
  app_timer_create(&led_timer, APP_TIMER_MODE_REPEATED, update_leds);
  app_timer_start(led_timer, 32, NULL);
  // set default state for the LED display (Part 4 and onwards)
}


