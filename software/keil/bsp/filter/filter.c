#include "filter.h"

float lowpass_filter(float input, float last_output, float alpha)
{
    return alpha * input + last_output * (1 - alpha);
}
