// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"

uint32_t row_leds[] = {LED_ROW1, LED_ROW2, LED_ROW3, LED_ROW4, LED_ROW5};
uint32_t col_leds[] = {LED_COL1, LED_COL2, LED_COL3, LED_COL4, LED_COL5};
bool led_states[5][5] = {false};
int nrow;
int nchar;
int strlg;
char string[6];
//char* string;

static void ledshuffle() {
  if (nrow == 0) {
    nrf_gpio_pin_clear(row_leds[4]);
  } else {
    nrf_gpio_pin_clear(row_leds[nrow - 1]);
  }

    nrf_gpio_pin_set(row_leds[nrow]);

  for (int i = 0; i < 5; i++) {
    if (led_states[nrow][i]) {
    	nrf_gpio_pin_clear(col_leds[i]);
    } else {
       nrf_gpio_pin_set(col_leds[i]);
    }
   }

  if (nrow > 3) {
    nrow = 0;
  } else {
    nrow++;
  }
}

void led_char(char chr) {
  int asciichar = (int)chr;
  for (int i = 0; i < 5; i++) {
    uint8_t val = font[asciichar][i];
    // printf("getting char %d \n", asciichar);
    for (int j = 0; j < 5; j++) {
      led_states[i][j] = ((val >> j) & 1);
      // printf("%d", led_states[i][j]);
    }
  }
}

//void led_str(char* str) {
void led_str(){
  nrow = 0;
  nchar = 0;
  //strlg = strlen(str);
  strlg = strlen(string);
  //string = str;
  //printf("string is %s \n", string);
}

static void led_str_intr() {
  if (nchar <= strlg) {
    // printf("nchar val %d \n", nchar);
    led_char(string[nchar]);
    nchar++;
  }
}

void led_matrix_init(void) {
  
  // initialize row pins and col pins

  for (int i = 0; i < 5; i++) {
    nrf_gpio_pin_dir_set(row_leds[i], NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(col_leds[i], NRF_GPIO_PIN_DIR_OUTPUT);
  }

  // set default values for pins

  for (int i = 0; i < 5; i++) {
    nrf_gpio_pin_clear(row_leds[i]);
    nrf_gpio_pin_clear(col_leds[i]);
  }

  /*
  // try setting leds
  nrf_gpio_pin_set(row_leds[2]);
  for (int i = 0; i < 3; i++) {
    nrf_gpio_pin_set(col_leds[i]);
    } */

  // test X
  /* for (int i = 0; i < 5; i++, j++, k--) {
    led_states[i][j] = true;
    led_states[i][k] = true;
    } */

  // initialize timer(s) (Part 3 and onwards)
  //app_timer_init();
  //app_timer_create(&timer1, APP_TIMER_MODE_REPEATED, ledshuffle);
  //app_timer_start(timer1, 0.004*32768, NULL);
  //app_timer_create(&timer2, APP_TIMER_MODE_REPEATED, led_str_intr);
  //app_timer_start(timer2, 2*32768, NULL);
  virtual_timer_start_repeated(4000, ledshuffle);
  virtual_timer_start_repeated(2000000, led_str_intr);
  
  // set default state for the LED display (Part 4 and onwards)
}

