#ifndef SENSOR_H
#define SENSOR_H

#include <ADS1X15.h>
#include "conf.h"
#include <vector>

struct ForceEventSample {
    uint64_t eventId;
    uint64_t timestamp;
    float reading;
};

struct ForceEvent {
    uint64_t eventId;
    uint8_t sampleCount;
    std::vector<ForceEventSample> samples;
};
bool initialize_force_sensor();
bool sample_force_sensor();

#endif