// LSM303AGR driver for Microbit_v2
//
// Initializes sensor and communicates over I2C
// Capable of reading temperature, acceleration, and magnetic field strength


#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "lsm303agr.h"
#include "nrf_delay.h"

// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;

// Helper function to perform a 1-byte I2C read of a given register
//
// i2c_addr - address of the device to read from
// reg_addr - address of the register within the device to read
//
// returns 8-bit read value
static uint8_t i2c_reg_read_init(uint8_t i2c_addr, uint8_t reg_addr) {
  uint8_t rx_buf = 0;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    //implement me
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0)
  };

  nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
  return rx_buf;
}

static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
  uint8_t rx_buf = 0;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    //implement me
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0)
  };

   nrf_twi_mngr_transaction_t trans = {
				      .callback = NULL,
				      .p_user_data = NULL,
				      .p_transfers = read_transfer,
				      .number_of_transfers = 2,
				      .p_required_twi_cfg = NULL
				      }; 
  
  //nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);

  APP_ERROR_CHECK(nrf_twi_mngr_schedule(i2c_manager, &trans));
  return rx_buf;
}

// Helper function to perform a 1-byte I2C write of a given register
//
// i2c_addr - address of the device to write to
// reg_addr - address of the register within the device to write
static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
  //TODO: implement me
  //Note: there should only be a single two-byte transfer to be performed
  uint16_t fulldat = ((uint16_t)data << 8) | (uint16_t)reg_addr;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    //implement me
    NRF_TWI_MNGR_WRITE(i2c_addr, &fulldat, 2, 0)
  };
  nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 1, NULL);
}

// Initialize and configure the LSM303AGR accelerometer/magnetometer
//
// i2c - pointer to already initialized and enabled twim instance
void lsm303agr_init(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;

  // ---Initialize Accelerometer---

  // Reboot acclerometer
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG5, 0x80);
  nrf_delay_ms(100); // needs delay to wait for reboot

  // Enable Block Data Update
  // Only updates sensor data when both halves of the data has been read
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG4, 0x80);

  // Configure accelerometer at 100Hz, normal mode (10-bit)
  // Enable x, y and z axes
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG1, 0x77);

  // Read WHO AM I register
  // Always returns the same value if working
  uint8_t result = i2c_reg_read_init(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_WHO_AM_I_REG);
  //check the result of the Accelerometer WHO AM I register
  if(result == 0x33){
    printf("Correct Accelerometer Whoami\n");
    result = 0;
  }
  else{
    printf("Incorrect Accelerometer Whoami, got value %x\n", result);
    for(int i = 0; i <= 0x6f; i++){
      uint8_t testy = i2c_reg_read_init(LSM303AGR_ACC_ADDRESS, i);
      if(testy != 0){
        printf("Register: %x = %x \n", i, testy);
      }
    }
  }

  // ---Initialize Magnetometer---

  // Reboot magnetometer
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_A, 0x40);
  nrf_delay_ms(300); // needs delay to wait for reboot

  // Enable Block Data Update
  // Only updates sensor data when both halves of the data has been read
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_C, 0x10);

  // Configure magnetometer at 100Hz, continuous mode
  i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_A, 0x0C);

  // Read WHO AM I register
  result = i2c_reg_read_init(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_WHO_AM_I_REG);
  //check the result of the Magnetometer WHO AM I register
  if((result) == 0x40){
    printf("Correct Magnetometer Whoami\n");
  }
  else{
    printf("Incorrect Magnetometer Whoami, got value %x\n", (uint8_t)result);
    for(uint8_t i = 0; i < 0x6f; i++){
      uint16_t testy = i2c_reg_read_init(LSM303AGR_MAG_ADDRESS, i);
      if(testy != 0){
        printf("Register: %x = %x \n", i, testy);
      }
    }
  }

  // ---Initialize Temperature---

  // Enable temperature sensor
  i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_CFG_REG, 0xC0);
}



//NOTE: The magnetometer is not working at all (all registers are blank), and the accelerometer occasionally doesn't work



// Read the internal temperature sensor
//
// Return measurement as floating point value in degrees C
float lsm303agr_read_temperature(void) {
  uint8_t lowX;
  uint8_t highX;

  lowX = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_L);
  highX = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_H);
  int16_t X = lowX + ((uint16_t)highX << 8);
  float temp = ((float)X)/256 + 25;

  return temp;
}

lsm303agr_measurement_t lsm303agr_read_accelerometer(void) {
  //TODO: implement me
  int16_t x_raw = (int16_t)((uint16_t)i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_X_L) + ((uint16_t)i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_X_H) << 8)) >> 6;
  //printf("%x\n", x_raw);
  int16_t y_raw = ((int16_t)((uint16_t)i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Y_L) | ((uint16_t)i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Y_H) << 8))) >> 6;
  //printf("%x\n", y_raw);
  int16_t z_raw = ((int16_t)((uint16_t)i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Z_L) + ((uint16_t)i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Z_H) << 8))) >> 6;
  float x = ((float)x_raw * 3.9)/1000;
  float y = ((float)y_raw * 3.9) * .001;
  float z = ((float)((float)z_raw * 3.9))/1000;

  lsm303agr_measurement_t measurement = {x, y, z};
  return measurement;
}

lsm303agr_measurement_t lsm303agr_read_tilt(void) {
  lsm303agr_measurement_t accelmeasure = lsm303agr_read_accelerometer();
  float phi = atan((sqrt((accelmeasure.x_axis)*(accelmeasure.x_axis) + (accelmeasure.y_axis)*(accelmeasure.y_axis)))/(accelmeasure.z_axis));
  float psi = atan((accelmeasure.y_axis)/(sqrt((accelmeasure.x_axis)*(accelmeasure.x_axis) + (accelmeasure.z_axis)*(accelmeasure.z_axis))));
  float theta = atan((accelmeasure.x_axis)/(sqrt((accelmeasure.y_axis)*(accelmeasure.y_axis) + (accelmeasure.z_axis)*(accelmeasure.z_axis))));
  phi = phi * 180 / M_PI;
  psi = psi * 180 / M_PI;
  theta = theta * 180 / M_PI;
  lsm303agr_measurement_t measurement = {theta, psi, phi};
  return measurement;
}

lsm303agr_measurement_t lsm303agr_read_magnetometer(void) {
  //TODO: implement me
  int16_t x_raw = (int16_t)(i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_X_L_REG) + (i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_X_H_REG) << 8));
  int16_t y_raw = (int16_t)(i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Y_L_REG) + (i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Y_H_REG) << 8));
  int16_t z_raw = ((int16_t)(i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Z_L_REG) + (i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Z_H_REG) << 8)));
  float x = ((float)x_raw * 1.5)*.1;
  float y = ((float)y_raw * 1.5)/10;
  float z = ((float)((float)z_raw * 1.5))/10;

  lsm303agr_measurement_t measurement = {x, y, z};

  return measurement;
}

