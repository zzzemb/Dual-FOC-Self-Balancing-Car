#include "blc_defs.h"

pid_t balance_pid;
pid_t speed_pid;
pid_t turn_pid;

void pid_config(pid_t *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

float pid_balance(pid_t *balance_pid, float cur_angle, float mechine_zero, float gyro)
{
    float err = cur_angle - mechine_zero;
    return err*balance_pid->kp + gyro * balance_pid->kd;
}

float pid_speed(pid_t *speed_pid, float cur_speed, float target_speed)
{
    static float speed_integral = 0;
    float speed_err = target_speed - cur_speed;
    speed_integral += speed_err * SPEED_DT;
    if(speed_integral >= 10)
        speed_integral = 10;
    if(speed_integral <= -10)
        speed_integral = -10;
    return speed_pid->kp * speed_err + speed_pid->ki * speed_integral;
}

float pid_turn(pid_t *turn_pid, float cur_turn, float target_turn)
{
    static float turn_integral = 0;
    static float last_turn;
    float turn_err = target_turn - cur_turn;
    
    float derivative = (cur_turn-last_turn) * SPEED_HZ;
    last_turn = cur_turn;
    
    turn_integral += turn_err * SPEED_DT;
    if(turn_integral >= 10)
        turn_integral = 10;
    if(turn_integral <= -10)
        turn_integral = -10;
    return turn_pid->kp * turn_err + turn_pid->ki * turn_integral - turn_pid->kd * derivative;
}


