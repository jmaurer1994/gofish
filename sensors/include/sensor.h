#ifndef SENSOR_H
#define SENSOR_H

#include "lib.h"
#include <HTTPClient.h>
#include "ADS1X15.h"
bool initialize_force_sensor();
void sample_force_sensor();
float read_force_sensor();

#endif