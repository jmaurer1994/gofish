#ifndef SENSOR_H
#define SENSOR_H

#include "lib.h"
#include <HTTPClient.h>
#include "ADS1X15.h"
void initialize_force_sensor();
float read_force_sensor();

#endif