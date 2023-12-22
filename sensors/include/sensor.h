#ifndef SENSOR_H
#define SENSOR_H

#include "conf.h"

#include <ADS1X15.h>
#include <vector>

bool initialize_force_sensor();
bool sample_force_sensor();

#endif