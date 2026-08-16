#ifndef FOC_H
#define FOC_H

#include "main.h"
#include "tim.h"
#include "adc.h"
#include "as5600.h"
#include "filter.h"
#include "arm_math.h"
#include "foc_defs.h"
#include "foc.h"

#define m1_rotor_phy_angle wrap_angle((m1_encoder_angle - m1_rotor_zero_angle)) 
#define m1_rotor_logic_angle wrap_angle((m1_rotor_phy_angle * POLE_PAIRS))

#define m2_rotor_phy_angle wrap_angle((m2_encoder_angle - m2_rotor_zero_angle)) 
#define m2_rotor_logic_angle wrap_angle((m2_rotor_phy_angle * POLE_PAIRS)) 

extern float m1_i_u;
extern float m1_i_v;
extern float m1_i_d;
extern float m1_i_q;
extern float m1_speed;
extern float m1_logic_angle;
extern float m1_encoder_angle;
extern float m1_rotor_zero_angle;

extern float m2_i_u;
extern float m2_i_v;
extern float m2_i_d;
extern float m2_i_q;
extern float m2_speed;
extern float m2_logic_angle;
extern float m2_encoder_angle;
extern float m2_rotor_zero_angle;

void m1_set_pwm_duty(float du, float dv, float dw);
void m2_set_pwm_duty(float du, float dv, float dw);
void m1_foc_forward(float d, float q, float rotor_rad);
void m2_foc_forward(float d, float q, float rotor_rad);
float cycle_diff(float diff, float cycle);
float wrap_angle(float angle);
void m1_pid_set_current(float pd, float id, float dd, 
    float pq, float iq, float dq);
void m1_foc_current_control(float d, float q);
void m2_pid_set_current(float pd, float id, float dd, 
    float pq, float iq, float dq);
void m2_foc_current_control(float d, float q);

void set_left_q(float q);
void set_right_q(float q);

extern float _diff;;
extern uint32_t _m2_u_v_raw;
extern uint16_t m1_u_v_raw;

#endif
