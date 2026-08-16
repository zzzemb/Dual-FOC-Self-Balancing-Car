#include "bmi160_port.h"

static struct bmi160_dev bmi160;
static struct bmi160_int_settg bmi160_settg;

struct bmi160_sensor_data accel;
struct bmi160_sensor_data gyro;
struct bmi160_sensor_data _accel;
struct bmi160_sensor_data raw_gyro;
float gyro_x_dps;
float gyro_y_dps;
float gyro_z_dps;

float angle_acc = 0;
float angle = 0;

float target_speed;
float target_turn;

int8_t bmi160_reg_read(uint8_t dev_addr,
                       uint8_t reg_addr,
                       uint8_t *data,
                       uint16_t len)
{
    uint8_t reg = reg_addr | 0x80;
    uint8_t dummy = 0xff;

    IMU_CS_L();

    HAL_SPI_Transmit(&hspi2, &reg, 1, 100);

    while(len--)
    {
        HAL_SPI_TransmitReceive(&hspi2,
                                &dummy,
                                data++,
                                1,
                                100);
    }

    IMU_CS_H();

    return 0;
}

int8_t bmi160_reg_write(uint8_t dev_addr,
                        uint8_t reg_addr,
                        uint8_t *data,
                        uint16_t len)
{
    uint8_t reg = reg_addr & 0x7F;

    IMU_CS_L();

    if(HAL_SPI_Transmit(&hspi2, &reg, 1, 100) != HAL_OK)
    {
        IMU_CS_H();
        return -1;
    }

    if(HAL_SPI_Transmit(&hspi2, data, len, 100) != HAL_OK)
    {
        IMU_CS_H();
        return -1;
    }

    IMU_CS_H();

    return 0;
}

HAL_StatusTypeDef bmi160_int_Init(void)
{
    bmi160_settg.int_pin_settg.output_en = BMI160_ENABLE;
    bmi160_settg.int_pin_settg.output_mode = BMI160_DISABLE;
    bmi160_settg.int_pin_settg.output_type = BMI160_ENABLE;
    bmi160_settg.int_pin_settg.edge_ctrl = BMI160_ENABLE;
    bmi160_settg.int_pin_settg.latch_dur = BMI160_LATCH_DUR_NONE;
    bmi160_settg.int_pin_settg.input_en = DISABLE;
    bmi160_settg.int_channel = BMI160_INT_CHANNEL_1;
    
    bmi160_settg.int_type = BMI160_ACC_GYRO_DATA_RDY_INT;
    
    int8_t rslt = bmi160_set_int_config(&bmi160_settg, &bmi160);
    
    return (rslt == BMI160_OK) ? HAL_OK : HAL_ERROR;
}

uint8_t imu_id = 0;

HAL_StatusTypeDef bmi160_config(void)
{
    int8_t rslt = BMI160_OK;
    
    bmi160.read     = bmi160_reg_read;
    bmi160.write    = bmi160_reg_write;
    bmi160.id       = 0;
    bmi160.delay_ms = HAL_Delay;
    bmi160.intf     = BMI160_SPI_INTF;
    
    uint8_t n = 0;
    while((rslt = bmi160_init(&bmi160)) != BMI160_OK && n < 10)
    {
        n++;
        HAL_Delay(100);
    }
    imu_id = rslt;
    if (rslt != BMI160_OK) return HAL_ERROR;
    
    bmi160.accel_cfg.odr = BMI160_ACCEL_ODR_200HZ;  
    bmi160.accel_cfg.range = BMI160_ACCEL_RANGE_4G;
    bmi160.gyro_cfg.odr = BMI160_GYRO_ODR_200HZ;    
    bmi160.gyro_cfg.range = BMI160_GYRO_RANGE_250_DPS;  
    bmi160.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4; 
    bmi160.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;    

    bmi160_set_sens_conf(&bmi160); 

    bmi160.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE; 
    bmi160.gyro_cfg.power  = BMI160_GYRO_NORMAL_MODE;
    
    bmi160_int_Init();
    
    bmi160_set_power_mode(&bmi160); 
    return (rslt == BMI160_OK) ? HAL_OK : HAL_ERROR;
}

void set_target_speed(float speed)
{
    target_speed = speed;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_11)
    {
        bmi160_get_sensor_data(BMI160_ACCEL_SEL | BMI160_GYRO_SEL,
            &_accel, &raw_gyro, &bmi160);
        gyro_x_dps = (float)raw_gyro.x / 131.2f;
        gyro_y_dps = (float)raw_gyro.y / 131.2f;
        gyro_z_dps = (float)raw_gyro.z / 131.2f;
        
        static float filter_alpha_accel = 0.5f;
        static float filter_alpha_gyro = 0.5f;
        _accel.x = lowpass_filter(_accel.x, accel.x, filter_alpha_accel);
        _accel.y = lowpass_filter(_accel.y, accel.y, filter_alpha_accel);
        _accel.z = lowpass_filter(_accel.z, accel.z, filter_alpha_accel);
        gyro_x_dps = lowpass_filter(gyro_x_dps, (float)raw_gyro.x / 131.2f, filter_alpha_gyro);
        gyro_y_dps = lowpass_filter(gyro_y_dps, (float)raw_gyro.y / 131.2f, filter_alpha_gyro);
        gyro_z_dps = lowpass_filter(gyro_z_dps, (float)raw_gyro.z / 131.2f, filter_alpha_gyro);
        
        accel.x = _accel.x;
        accel.y = _accel.y;
        accel.z = _accel.z;
        
        angle_acc = atan2f(accel.x, accel.z) * 57.2958f;
        static uint8_t once = 1;
        if(once)
        {
            once = 0;
            angle = angle_acc;
            return ;
        }
        angle += gyro_y_dps * DELTA_T;
        angle = 0.90f * angle + 0.1f * angle_acc;
        
        float balance_q = pid_balance(&balance_pid, angle, MECHINE_ZERO, -gyro_y_dps);
        static uint8_t n = 0;
        static float speed_q = 0;
        static float turn_q = 0;
        if(++n >= IMU_HZ / SPEED_HZ)
        {
            n = 0;
            float cur_speed = (m1_speed * MOTOR_DIR_LEFT + m2_speed * MOTOR_DIR_RIGHT) / 2.0f;
            speed_q = pid_speed(&speed_pid, cur_speed, target_speed);
            
            float cur_turn = (m1_speed * MOTOR_DIR_LEFT - m2_speed * MOTOR_DIR_RIGHT) / 2.0f;
            turn_q =  pid_turn(&turn_pid, cur_turn, target_turn);
        }
        
        float m1_target_q = balance_q - speed_q + turn_q;
        float m2_target_q = balance_q - speed_q - turn_q;

        if(angle > 45 || angle < -45)
        {
            pid_config(&balance_pid, 0, 0, 0);
            pid_config(&speed_pid, 0, 0, 0);
            set_left_q(0);
            set_right_q(0);
        }
        else
        {
            set_left_q(m1_target_q * MOTOR_DIR_LEFT);
            set_right_q(m2_target_q * MOTOR_DIR_RIGHT);
        }

    }
}
