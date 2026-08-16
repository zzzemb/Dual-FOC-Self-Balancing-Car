#pragma once
#ifndef PI
#define PI 3.14159265358979
#endif
#define deg2rad(a) (PI * (a) / 180.0f)
#define rad2deg(a) (180 * (a) / PI)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define POLE_PAIRS 7 
#define R_SHUNT 0.02f 
#define OP_GAIN 50.0f  
#define MAX_CURRENT 4.0f 
#define ADC_REF_VOLT 3.3f 
#define ADC_BITS 12 
