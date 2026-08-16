#include "foc.h"
#include <stdbool.h>
#include "math.h"

#define rad60 deg2rad(60)
#define SQRT3 1.73205080756887729353f

float m1_i_u;
float m1_i_v;
float m1_i_d;
float m1_i_q;
float m1_speed; 
float m1_logic_angle;
float m1_encoder_angle;
float m1_rotor_zero_angle;

float m2_i_u;
float m2_i_v;
float m2_i_d;
float m2_i_q;
float m2_speed; 
float m2_logic_angle;
float m2_encoder_angle;
float m2_rotor_zero_angle;

float left_i_q;
float right_i_q;

static arm_pid_instance_f32 m1_pid_current_d;
static arm_pid_instance_f32 m1_pid_current_q;

static arm_pid_instance_f32 m2_pid_current_d;
static arm_pid_instance_f32 m2_pid_current_q;

void m1_set_pwm_duty(float du, float dv, float dw)
{
    du = max(0.0f, min(du, 0.9f));
    dv = max(0.0f, min(dv, 0.9f));
    dw = max(0.0f, min(dw, 0.9f));
    __disable_irq();
    TIM1->CCR2 = du * TIM1->ARR;
    TIM1->CCR3 = dv * TIM1->ARR;
    TIM1->CCR4 = dw * TIM1->ARR;
    __enable_irq();
}

void m2_set_pwm_duty(float du, float dv, float dw)
{
    du = max(0.0f, min(du, 0.9f));
    dv = max(0.0f, min(dv, 0.9f));
    dw = max(0.0f, min(dw, 0.9f));
    __disable_irq();
    TIM8->CCR1 = du * TIM8->ARR;
    TIM8->CCR2 = dv * TIM8->ARR;
    TIM8->CCR3 = dw * TIM8->ARR;
    __enable_irq();
}

static void svpwm(float phi, float d, float q, 
    float *d_u, float *d_v, float *d_w)
{
    d = min(d, 1);
    q = min(q, 1);
    d = max(d, -1);
    q = max(q, -1);
    
    const uint8_t v[6][3] = {{1, 0, 0}, {1, 1, 0}, {0, 1, 0},
                             {0, 1, 1}, {0, 0, 1}, {1, 0, 1}};
    const uint8_t k2sector[] = {4, 6, 5, 5, 3, 1, 2, 2};
    float sin_phi = arm_sin_f32(phi);
    float cos_phi = arm_cos_f32(phi);
    float alpha = 0;
    float beta = 0;
    arm_inv_park_f32(d, q, &alpha, &beta, sin_phi, cos_phi);
    
    bool A = beta > 0;
    bool B = fabs(beta) > SQRT3 * fabs(alpha);
    bool C = alpha > 0;

    uint8_t K = 4 * A + 2 * B + C;
    uint8_t sector = k2sector[K];
    
    float t_m = arm_sin_f32(sector * rad60) * alpha - arm_cos_f32(sector * rad60) * beta;
    float t_n = beta * arm_cos_f32(sector * rad60 - rad60) - alpha * arm_sin_f32(sector * rad60 - rad60);
    float t_0 = 1.0f - t_m - t_n;

    *d_u = t_m * v[sector - 1][0] + t_n * v[sector % 6][0] + t_0 / 2;
    *d_v = t_m * v[sector - 1][1] + t_n * v[sector % 6][1] + t_0 / 2;
    *d_w = t_m * v[sector - 1][2] + t_n * v[sector % 6][2] + t_0 / 2;    
}

void m1_foc_forward(float d, float q, float rotor_rad)
{
    float du = 0;
    float dv = 0;
    float dw = 0;
    svpwm(rotor_rad, d, q, &du, &dv, &dw);
    m1_set_pwm_duty(du, dv, dw);
}

void m2_foc_forward(float d, float q, float rotor_rad)
{
    float du = 0;
    float dv = 0;
    float dw = 0;
    svpwm(rotor_rad, d, q, &du, &dv, &dw);
    m2_set_pwm_duty(du, dv, dw);
}

float cycle_diff(float diff, float cycle)
{
    if(diff > (cycle / 2.0f))
        diff -= cycle;
    else if(diff < -(cycle / 2.0f))
        diff += cycle;
    return diff;
}

float wrap_angle(float angle) {
    float res = fmodf(angle, 2.0f * PI);
    if (res < 0.0f) {
        res += 2.0f * PI;
    }
    return res;
}

void set_left_q(float q)
{
    left_i_q = q;
}

void set_right_q(float q)
{
    right_i_q = q;
}

uint32_t _m2_u_v_raw;


void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC1)
    {
        float m1_u_u = ADC_REF_VOLT * ((float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) / (4095.0f) - 0.5f);
        float m1_u_v = ADC_REF_VOLT * ((float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) / (4095.0f) - 0.5f);
        
        float _m1_i_u = m1_u_u / R_SHUNT / OP_GAIN;
        float _m1_i_v = m1_u_v / R_SHUNT / OP_GAIN;
        
        m1_i_u  = _m1_i_u;
        m1_i_v  = _m1_i_v;
        
        float m1_i_alpha = 0;
        float m1_i_beta = 0;
        
        arm_clarke_f32(m1_i_u, m1_i_v, &m1_i_alpha, &m1_i_beta);
        float _m1_i_d = 0;
        float _m1_i_q = 0;
        float sinval = arm_sin_f32(m1_rotor_logic_angle); 
        float cosval = arm_cos_f32(m1_rotor_logic_angle);
        
        arm_park_f32(m1_i_alpha, m1_i_beta, &_m1_i_d, &_m1_i_q, sinval, cosval);
        
        static float m1_filter_i_d = 1.0f;
        static float m1_filter_i_q = 1.0f;
        m1_i_d = lowpass_filter(_m1_i_d, m1_i_d, m1_filter_i_d);
        m1_i_q = lowpass_filter(_m1_i_q, m1_i_q, m1_filter_i_q);
        
        m1_foc_current_control(0, left_i_q);
                
    }
    if(hadc->Instance == ADC2)
    {
        _m2_u_v_raw = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_2);
        float m2_u_u = ADC_REF_VOLT * ((float)HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1) / (4095.0f) - 0.5f);
        float m2_u_v = ADC_REF_VOLT * ((float)_m2_u_v_raw / (4095.0f) - 0.5f);
        
        float _m2_i_u = m2_u_u / R_SHUNT / OP_GAIN;
        float _m2_i_v = m2_u_v / R_SHUNT / OP_GAIN;
        
        m2_i_u  = _m2_i_u;
        m2_i_v  = _m2_i_v;
        
        float m2_i_alpha = 0;
        float m2_i_beta = 0;
        
        arm_clarke_f32(m2_i_u, m2_i_v, &m2_i_alpha, &m2_i_beta);
        float _m2_i_d = 0;
        float _m2_i_q = 0;
        float sinval = arm_sin_f32(m2_rotor_logic_angle);
        float cosval = arm_cos_f32(m2_rotor_logic_angle);
        
        arm_park_f32(m2_i_alpha, m2_i_beta, &_m2_i_d, &_m2_i_q, sinval, cosval);
        
        static float m2_filter_i_d = 1.0f;
        static float m2_filter_i_q = 1.0f;
        m2_i_d = lowpass_filter(_m2_i_d, m2_i_d, m2_filter_i_d);
        m2_i_q = lowpass_filter(_m2_i_q, m2_i_q, m2_filter_i_q);
        
        m2_foc_current_control(0, right_i_q);
    }
}

void m1_pid_set_current(float pd, float id, float dd, 
    float pq, float iq, float dq)
{
    m1_pid_current_d.Kp = pd;
    m1_pid_current_d.Ki = id;
    m1_pid_current_d.Kd = dd;

    m1_pid_current_q.Kp = pq;
    m1_pid_current_q.Ki = iq;
    m1_pid_current_q.Kd = dq;
    
    arm_pid_init_f32(&m1_pid_current_d, false);
    arm_pid_init_f32(&m1_pid_current_q, false);
}

static float m1_current_d_loop(float d)
{
    float diff = d - m1_i_d;
    m1_pid_current_d.state[2] = fmaxf(fminf(m1_pid_current_d.state[2], 0.9f), -0.9f);
    return arm_pid_f32(&m1_pid_current_d, diff);
}

static float m1_current_q_loop(float q)
{
    float diff = q - m1_i_q;
    m1_pid_current_q.state[2] = fmaxf(fminf(m1_pid_current_q.state[2], 0.9f), -0.9f);
    return arm_pid_f32(&m1_pid_current_q, diff);
}

void m1_foc_current_control(float d, float q)
{
    float _d = m1_current_d_loop(d);
    float _q = m1_current_q_loop(q);
    m1_foc_forward(_d, _q, m1_rotor_logic_angle);
}

void m2_pid_set_current(float pd, float id, float dd, 
    float pq, float iq, float dq)
{
    m2_pid_current_d.Kp = pd;
    m2_pid_current_d.Ki = id;
    m2_pid_current_d.Kd = dd;

    m2_pid_current_q.Kp = pq;
    m2_pid_current_q.Ki = iq;
    m2_pid_current_q.Kd = dq;
    
    arm_pid_init_f32(&m2_pid_current_d, false);
    arm_pid_init_f32(&m2_pid_current_q, false);
}

float _diff;

static float m2_current_d_loop(float d)
{
    float diff = d - m2_i_d;
    m2_pid_current_d.state[2] = fmaxf(fminf(m2_pid_current_d.state[2], 0.9f), -0.9f);
    return (_diff = arm_pid_f32(&m2_pid_current_d, diff));
}

static float m2_current_q_loop(float q)
{
    float diff = q - m2_i_q;
    m2_pid_current_q.state[2] = fmaxf(fminf(m2_pid_current_q.state[2], 0.9f), -0.9f);
    return arm_pid_f32(&m2_pid_current_q, diff);
}

void m2_foc_current_control(float d, float q)
{
    float _d = m2_current_d_loop(d);
    float _q = m2_current_q_loop(q);
    m2_foc_forward(_d, _q, m2_rotor_logic_angle);
}


