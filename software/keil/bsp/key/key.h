#ifndef KEY_H
#define KEY_H

#include "main.h"
#include "gpio.h"

enum key_val{
    KEY_NONE = 0,
    KEY_UP = 1,
    KEY_DOWN = 2,
};
    
enum key_val key_read(void);

#endif
