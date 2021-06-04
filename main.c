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
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "nrf.h"
#include "nrfx_pwm.h"
#include "nrfx_saadc.h"

#define LED_RED   EDGE_P13
#define LED_GREEN EDGE_P14
#define LED_BLUE  EDGE_P15

#define SWITCH_IN TOUCH_LOGO

bool light_on = false;
uint32_t last_on = 0;
uint32_t timer_start;

// Global variables
//APP_TIMER_DEF(sample_timer);
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);


// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// Holds duty cycle values to trigger PWM toggle
nrf_pwm_values_common_t sequence_data[1] = {0};

// Sequence structure for configuring DMA
nrf_pwm_sequence_t pwm_sequence = {
  .values.p_common = sequence_data,
  .length = 1,
  .repeats = 0,
  .end_delay = 0,
};


static void pwm_init(void) {
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 500 kHz, count mode to Up, and load mode to Common
  // The Countertop value doesn't matter for now. We'll set it in play_tone()
  nrfx_pwm_config_t configurator;
  configurator.output_pins[0] = SPEAKER_OUT;
  configurator.output_pins[1] = NRFX_PWM_PIN_NOT_USED;
  configurator.output_pins[2] = NRFX_PWM_PIN_NOT_USED;
  configurator.output_pins[3] = NRFX_PWM_PIN_NOT_USED;
  configurator.base_clock = NRF_PWM_CLK_500kHz;
  configurator.count_mode = NRF_PWM_MODE_UP;
  configurator.top_value = 0;
  configurator.load_mode = NRF_PWM_LOAD_COMMON;
  configurator.step_mode = NRF_PWM_STEP_AUTO;
  nrfx_pwm_init(&PWM_INST, &configurator, NULL);
}

static void play_tone(uint16_t frequency) {
  // Stop the PWM (and wait until its finished)

  // Set a countertop value based on desired tone frequency
  // You can access it as NRF_PWM0->COUNTERTOP
  NRF_PWM0->COUNTERTOP = 500000/frequency;

  // Modify the sequence data to be a 25% duty cycle
  sequence_data[0] = (NRF_PWM0->COUNTERTOP)/2;

  // Start playback of the samples and loop indefinitely
   nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

// Capacitive touch sensor
// output, write 0, input, take time stamp
// timestamp now and when interrupt occurs, subtract to find time, use read_time
static void touch_sensing(void) {
 nrf_drv_gpiote_in_event_disable(SWITCH_IN);
 nrf_gpio_cfg_output(SWITCH_IN);
 nrf_gpio_pin_clear(SWITCH_IN);
 timer_start = read_timer();
 //printf("init time: %ld \n", timer_start);
 nrf_gpio_cfg_input(SWITCH_IN, NRF_GPIO_PIN_NOPULL);
 nrf_drv_gpiote_in_event_enable(SWITCH_IN, true);
 }

// Handles interrupts for the capacitive touch sensor
static void interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
  uint32_t timer_interrupt = read_timer();
  //printf("interrupt time: %ld \n", timer_interrupt);
  uint32_t diff = timer_interrupt - timer_start;
  printf("difference: %ld \n", diff);
  if (diff > 2000) {
    if(timer_interrupt - last_on > 500000){
      last_on = timer_interrupt;
    if (light_on) {
      light_on = false;
      printf("Switch: OFF\n");
    } else {
      light_on = true;
      printf("Switch: ON\n");
    }
  }
  }
  nrf_gpio_cfg_input(SWITCH_IN, NRF_GPIO_PIN_NOPULL);
  touch_sensing();
}


static void gpio_init(void) {
  // Initialize output pins
  nrf_gpio_cfg_output(LED_RED);
  nrf_gpio_cfg_output(LED_GREEN);
  nrf_gpio_cfg_output(LED_BLUE);  

  // Set LEDs off initially
  nrf_gpio_pin_set(LED_RED);
  nrf_gpio_pin_set(LED_BLUE);
  nrf_gpio_pin_set(LED_GREEN);

  nrf_gpio_pin_clear(SWITCH_IN);
  ret_code_t err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_config_t pin_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
  pin_config.pull = NRF_GPIO_PIN_NOPULL;

  // Initializes input pins
  err_code = nrf_drv_gpiote_in_init(SWITCH_IN, &pin_config, interrupt_handler);
  APP_ERROR_CHECK(err_code);
  NVIC_EnableIRQ(GPIOTE_IRQn);
  NVIC_SetPriority(GPIOTE_IRQn, 7);
  nrf_drv_gpiote_in_event_enable(SWITCH_IN, true);
}




// Callback function to check the temperature
static void check_temp_and_accel(void) {
  lsm303agr_measurement_t tempval = TILT;
  printf("Temp: %f\n", lsm303agr_read_temperature()); // For Debugging
  lsm303agr_read_temperature();
  lsm303agr_read_tilt();
  // Check accelerometer for large tilt- set off alarm if too much tilt
    if(fabs((int)(tempval.z_axis - TILT.z_axis)) > 20){
    printf("tilt: %f, %f \n", tempval.z_axis, TILT.z_axis);
    // while(nrf_gpio_pin_read(SWITCH_IN)){
    while (fabs((int)(tempval.z_axis - TILT.z_axis)) > 20){
      play_tone(554);
      nrf_delay_ms(500);
      play_tone(659);
      nrf_delay_ms(500);
      lsm303agr_read_tilt();
       }
    nrfx_pwm_stop(&PWM_INST, true);
  }

  // Change lamp based on temperature
  if(!light_on){
    nrf_gpio_pin_set(LED_BLUE);
    nrf_gpio_pin_set(LED_GREEN);
    nrf_gpio_pin_set(LED_RED);
  }
  // If the lamp is on, it changes depending on how hot it is
  else{
    nrf_gpio_pin_clear(LED_BLUE);
    if(TEMP > 25){
      nrf_gpio_pin_set(LED_BLUE);
      nrf_gpio_pin_set(LED_RED);
      nrf_gpio_pin_clear(LED_GREEN);
    }
    else{
      nrf_gpio_pin_set(LED_GREEN);
    }
    if(TEMP > 27){
      nrf_gpio_pin_set(LED_BLUE);
      nrf_gpio_pin_set(LED_GREEN);
      nrf_gpio_pin_clear(LED_RED);
    }
    else{
      nrf_gpio_pin_set(LED_RED);
    }
  }
}

// Callback function to print the temperature on the LED matrix
static void print_temp(void) {
  char str[50];
  snprintf(str, sizeof(str), "%.2f  ", TEMP); // Prints with 2 spaces to make the end longer
  new_print_string(str);
}

int main(void) {
  printf("Board started!\n");

  // initialize PWM
  pwm_init();
  
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

  // initialize app timers
  app_timer_init();


  // Initialize and start virtual timers
  virtual_timer_init();
  virtual_timer_start_repeated(1000000, check_temp_and_accel);
  virtual_timer_start_repeated(2000000, print_temp);
  //virtual_timer_start_repeated(100000, touch_sensing);
  touch_sensing();
  // Start off by doing an initialized printing of the temp then afterward it just updates the temp without changing the timer
  char str[50];
  snprintf(str, sizeof(str), "%.2f  ", TEMP);
  init_print_string(str, 0.5);
  // loop forever
  while (1) {
    // Don't put any code in here. Instead put periodic code in `sample_timer_callback()`
    nrf_delay_ms(1000);
  }
}

