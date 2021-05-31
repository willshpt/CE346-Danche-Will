
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "lsm303agr.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "led_matrix.h"
#include "microbit_v2.h"
#include "virtual_timer.h"
#include "virtual_timer_linked_list.h"
#include "nrf_twi_mngr.h"
#include "nrf.h"
#include "nrfx_pwm.h"
#include "nrfx_saadc.h"

#define LED_RED   EDGE_P13
#define LED_GREEN EDGE_P14
#define LED_BLUE  EDGE_P15

#define SWITCH_IN TOUCH_LOGO

// Global variables
APP_TIMER_DEF(sample_timer);
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

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

// Touch lamp should have it's own hardware interrupt or a much faster timer- moving to interrupt
static void sample_timer_callback(void* _unused) {
  if(nrf_gpio_pin_read(SWITCH_IN)){
    //nrf_gpio_pin_clear(LED_BLUE);
    printf("Switch: OFF\n");
  }
  else{
    //nrf_gpio_pin_set(LED_BLUE);
    printf("Switch: ON\n");
  }
}

// Callback function to check the temperature
static void check_temp_and_accel(void) {
  printf("Temp: %f\n", lsm303agr_read_temperature()); // For Debugging
  lsm303agr_read_temperature();
  lsm303agr_read_accelerometer();
  //lsm303agr_read_temperature();
   //list_print();
}

// Callback function to print the temperature on the LED matrix
static void print_temp(void) {
  //printf("PRINTING...\n");
  char str[50];
  snprintf(str, sizeof(str), "%.2f  ", TEMP); // Prints with 2 spaces to make the end longer
  new_print_string(str);
}

int main(void) {
  printf("Board started!\n");
  
  // initialize GPIO
  gpio_init();

  // Initialize I2C peripheral and driver
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = I2C_SCL;
  i2c_config.sda = I2C_SDA;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);

  // Initialize the LSM303AGR accelerometer/magnetometer sensor
  lsm303agr_init(&twi_mngr_instance);

  // initialize LED matrix
  led_matrix_init();

  // initialize ADC
  //adc_init();

  // initialize app timers
  app_timer_init();
  app_timer_create(&sample_timer, APP_TIMER_MODE_REPEATED, sample_timer_callback);

  // start timer
  // change the rate to whatever you want
  app_timer_start(sample_timer, 0.5*32768, NULL);

  virtual_timer_init();
  nrf_delay_ms(1000);
  virtual_timer_start_repeated(1000000, check_temp_and_accel);
  virtual_timer_start_repeated(2000000, print_temp);
  // Start off by doing an initialized printing of the temp
  char str[50];
  snprintf(str, sizeof(str), "%.2f  ", TEMP);
  init_print_string(str, 0.5);
  

  // loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
    nrf_delay_ms(1000);
    //list_print();
  }
}

