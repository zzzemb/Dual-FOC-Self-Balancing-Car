# Dual-FOC-Self-Balancing-Car
A dual-FOC self-balancing car powered by two 2208 BLDC motors and STM32G474CBT6, with FOC control implemented by hand.

The project implements dual motor FOC control from scratch, including current control, velocity control and balance control.

## Features

- Dual BLDC motor FOC control
- FOC implemented from scratch
- Encoder-based motor feedback
- Cascaded PID control:
  - Current loop
  - Velocity loop
  - Balance loop
  - Wheel synchronization compensation
- 2.4GHz wireless remote control (WIP)

## Hardware

### Main Controller

- MCU: STM32G474CBT6
- Motor Driver: DRV8318
- Current Sense: INA199A1
- IMU: BMI160
- Wireless Communication: SI24R1

- 
## Control

Implemented:

- Clarke transform
- Park transform
- Inverse Park transform
- SVPWM modulation
- Current PI control
- Balance PID control
- Velocity PI control
- Differential compensation control

## Status

Currently under development.

Completed:
- Dual motor FOC control
- Self-balancing control
- Velocity control
- Basic motion control

TODO:
- Wireless remote control
- Further tuning
- Mechanical optimization
