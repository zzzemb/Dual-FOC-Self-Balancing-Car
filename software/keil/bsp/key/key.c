#include "key.h"

enum key_val key_read(void)
{
    static uint8_t key_up_last = GPIO_PIN_SET;
    uint8_t key_up_now = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
    if(key_up_last != key_up_now)
    {
        key_up_last = key_up_now;
        if(key_up_now == GPIO_PIN_SET)
            return KEY_UP;
    }
    
    static uint8_t key_down_last = GPIO_PIN_SET;
    uint8_t key_down_now = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
    if(key_down_last != key_down_now)
    {
        key_down_last = key_down_now;
        if(key_down_now == GPIO_PIN_SET)
            return KEY_DOWN;
    }
    return KEY_NONE;
}

