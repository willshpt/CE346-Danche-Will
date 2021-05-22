
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "led_matrix.h"
#include "microbit_v2.h"
#include "nrf.h"
#include "nrfx_pwm.h"
#include "nrfx_saadc.h"

#define LED_RED   EDGE_P13
#define LED_GREEN EDGE_P14
#define LED_BLUE  EDGE_P15

#define SWITCH_IN TOUCH_LOGO

// Global variables
APP_TIMER_DEF(sample_timer);

static void gpio_init(void) {
  // Initialize output pins
  // TODO
  nrf_gpio_cfg_output(LED_RED);
  nrf_gpio_cfg_output(LED_GREEN);
  nrf_gpio_cfg_output(LED_BLUE);  

  // Set LEDs off initially
  // TODO
  nrf_gpio_pin_set(LED_RED);
  nrf_gpio_pin_set(LED_BLUE);
  nrf_gpio_pin_set(LED_GREEN);

  // Initialize input pin
  // TODO
  nrf_gpio_cfg_input(SWITCH_IN, NRF_GPIO_PIN_NOPULL);
}

static void sample_timer_callback(void* _unused) {
  if(nrf_gpio_pin_read(SWITCH_IN)){
    //nrf_gpio_pin_clear(LED_BLUE);
    printf("Switch: ON\n");
  }
  else{
    //nrf_gpio_pin_set(LED_BLUE);
    printf("Switch: OFF\n");
  }
}


int main(void) {
  printf("Board started!\n");
  
  // initialize GPIO
  gpio_init();

  // initialize ADC
  //adc_init();

  // initialize app timers
  app_timer_init();
  app_timer_create(&sample_timer, APP_TIMER_MODE_REPEATED, sample_timer_callback);

  // start timer
  // change the rate to whatever you want
  app_timer_start(sample_timer, 32768, NULL);


  // loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
    nrf_delay_ms(1000);
  }
}

