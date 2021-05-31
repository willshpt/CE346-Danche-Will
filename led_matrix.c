// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"
#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"

// Timer for LEDs
APP_TIMER_DEF(my_timer_1);
// Array of LED states stored as uint8_t for ease
uint8_t led_states[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
// Current row of the matrix
uint8_t current_row = 0;
// Current string being printed
char current_string[50]; // Limited at 50, anything above is unneccessary
char next_string[50];
uint8_t current_string_length = 0;
uint8_t next_string_length = 0;
uint8_t current_char = 0;
bool need_space = false; // Used to make space between values
// Virtual timer address
uint32_t vtimer = NULL;

// Function to print a char
void print_char(char c){
  update_matrix(font[c]);
}

// Callback function for timer, ends timer after printing whole string
void print_string(){
  if(!need_space){
    print_char(current_string[current_char]);
    current_char++;
  }
  else{
    print_char(' ');
  }
  need_space = !need_space;
  if(current_char >= current_string_length || current_string[current_char] == 0){
    current_char = 0;
    int i = 0;
    while(next_string[i] != 0 && i < 50){
      current_string[i] = next_string[i];
      i++;
    }
    current_string_length = i;
  }
}

// When given a string, prints it with that letter delay using virtual timer
void init_print_string(char* str, double letter_delay){
  int i = 0;
  // Load the new value in- may not need while loop, but I'm just rushing at this point
  while(str[i] != 0 && i < 50){
    current_string[i] = str[i];
    next_string[i] = str[i];
    i++;
  }
  current_string_length = i;
  next_string_length = i;
  current_char = 0;
  vtimer = virtual_timer_start_repeated((uint32_t)(letter_delay * 500000), print_string);
}

void new_print_string(char* str){
  int i = 0;
  // Load the new value in- may not need while loop, but I'm just rushing at this point
  while(str[i] != 0 && i < 50){
    next_string[i] = str[i];
    i++;
  }
  next_string_length = i;
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

static void led0_toggle(void* _unused) {
    update_leds();
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
  //virtual_timer_init();
  virtual_timer_start_repeated(32, update_leds);
  //app_timer_init();
  //app_timer_create(&my_timer_1, APP_TIMER_MODE_REPEATED, led0_toggle);
  //app_timer_start(my_timer_1, 32, NULL);
  // set default state for the LED display (Part 4 and onwards)
}


