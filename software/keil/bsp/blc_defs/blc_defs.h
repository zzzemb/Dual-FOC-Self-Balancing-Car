#pragma once

#define IMU_HZ 200
#define DELTA_T (1.0f / 200.0f)

#define SPEED_HZ 20.0f 
#define SPEED_DT (1.0f / SPEED_HZ) 

typedef struct _pid {
    float kp;
    float ki;
    float kd;
} pid_t;

extern pid_t balance_pid;
extern pid_t speed_pid;
extern pid_t turn_pid;

void pid_config(pid_t *pid, float kp, float ki, float kd);
float pid_balance(pid_t *balance_pid, float cur_angle, float mechine_zero, float gyro);
float pid_speed(pid_t *speed_pid, float cur_speed, float target_speed);
float pid_turn(pid_t *turn_pid, float cur_turn, float target_turn);

#define MECHINE_ZERO 3.20f
#define MOTOR_DIR_LEFT  1
#define MOTOR_DIR_RIGHT -1

