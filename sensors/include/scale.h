#ifndef SCALE_H
#define SCALE_H

#include "conf.h"
#include <HX711.h>

bool initialize_scale();
void tare_scale_by_value();
void reset_tare_value();
void zero_tare_value();
double get_avg_reading(long readings);
#endif