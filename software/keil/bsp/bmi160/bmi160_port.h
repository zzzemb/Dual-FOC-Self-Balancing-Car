#ifndef BMI160_H
#define BMI160_H

#include "main.h"
#include "spi.h"
#include "bmi160.h"
#include "bmi160_defs.h"
#include "filter.h"
#include "blc_defs.h"
#include "foc.h"

extern struct bmi160_dev bmi160;

extern struct bmi160_sensor_data accel;
extern struct bmi160_sensor_data gyro;

extern uint8_t imu_id;
extern float angle;
extern float gyro_y_dps;

#define IMU_CS_H() do{HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);}while(0)
#define IMU_CS_L() do{HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);}while(0)

HAL_StatusTypeDef bmi160_config(void);
void set_target_speed(float speed);

#endif
