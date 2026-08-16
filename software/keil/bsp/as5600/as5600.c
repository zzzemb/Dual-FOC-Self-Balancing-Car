#include "as5600.h"

as5600_t as5600_dat;

HAL_StatusTypeDef as5600_init(Mx mx)
{
    if(mx == m1)
        return HAL_I2C_IsDeviceReady(&m1i2c, AS_ADDR, 3, 200);
    else if(mx == m2)
        return HAL_I2C_IsDeviceReady(&m2i2c, AS_ADDR, 3, 200);
    return HAL_ERROR;
}

uint16_t as5600_readRaw(Mx mx)
{
    uint8_t buf[2];
    if(mx == m1)
        HAL_I2C_Mem_Read(&m1i2c, AS_ADDR, ANGLE_H_REG, 
            I2C_MEMADD_SIZE_8BIT, buf, 2, 200);
    else if(mx == m2)
        HAL_I2C_Mem_Read(&m2i2c, AS_ADDR, ANGLE_H_REG, 
            I2C_MEMADD_SIZE_8BIT, buf, 2, 200);
    
    uint16_t raw = ((buf[0] & 0x0F) << 8) | buf[1];
    return raw;
}

float as5600_readAngle(Mx mx)
{
    float angle;
    angle = as5600_readRaw(mx);
    return angle * 2.0f * PI / 4096.0f;
}

void as5600_read(as5600_t *as_dat)
{
    as_dat->m1_as_dat = as5600_readAngle(m1);
    as_dat->m2_as_dat = as5600_readAngle(m2);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        as5600_read(&as5600_dat);
        
        m1_encoder_angle = as5600_dat.m1_as_dat;
        static float m1_encoder_angle_last = 0;
        static uint8_t m1_once = 1;
        if(m1_once)
        {
            m1_once = 0;
            m1_encoder_angle_last = as5600_dat.m1_as_dat;
        }
        float m1_diff_angle = cycle_diff(as5600_dat.m1_as_dat - m1_encoder_angle_last, 2 * PI);
        m1_encoder_angle_last = as5600_dat.m1_as_dat;
        float _m1_speed = m1_diff_angle * 1000;
        float filter_alpha = 0.1f;
        m1_speed = lowpass_filter(_m1_speed, m1_speed, filter_alpha);
        
        m2_encoder_angle = as5600_dat.m2_as_dat;
        static float m2_encoder_angle_last = 0;
        static uint8_t m2_once = 1;
        if(m2_once)
        {
            m2_once = 0;
            m2_encoder_angle_last = as5600_dat.m2_as_dat;
        }
        float m2_diff_angle = cycle_diff(as5600_dat.m2_as_dat - m2_encoder_angle_last, 2 * PI);
        m2_encoder_angle_last = as5600_dat.m2_as_dat;
        float _m2_speed = m2_diff_angle * 1000;
        m2_speed = lowpass_filter(_m2_speed, m2_speed, filter_alpha);
    }
}
