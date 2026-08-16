#ifndef AS5600_H
#define AS5600_H

#include "main.h"
#include "i2c.h"
#include "foc.h"
#include "filter.h"

typedef struct _as5600_dat{
    float m1_as_dat; 
    float m2_as_dat; 
} as5600_t;

typedef enum _Mx {
    m1,
    m2,
} Mx;

#define m1i2c hi2c1
#define m2i2c hi2c3

#define AS_ADDR (0x36 << 1) 
#define ANGLE_H_REG 0x0C
#define ANGLE_L_REG 0x0D

extern as5600_t as5600_dat;

HAL_StatusTypeDef as5600_init(Mx mx);
void as5600_read(as5600_t *as_dat);

#endif
