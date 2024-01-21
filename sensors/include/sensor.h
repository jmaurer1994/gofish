#ifndef SENSOR_H
#define SENSOR_H

#include "conf.h"
#include "network.h"

#include <ADS1X15.h>
#include <vector>

class ForceEvent {
    uint64_t timestamp;
    std::vector<float> samples;

  public:
    void record_sample(float value) { samples.push_back(value); }
    void init_object() { timestamp = get_epoch_time(); }
    void reset_object() { samples.clear(); }
    uint64_t get_timestamp() const { return timestamp; }
    std::vector<float> get_samples() { return samples; }
    int samples_collected() { return samples.size(); }
    float peak_force() {
        return *std::max_element(samples.begin(), samples.end());
    }
    float average_force() {
        float total = 0;
        for (float s : samples) {
            total += s;
        }
        return total / samples_collected();
    }
};

bool initialize_force_sensor();
bool sample_force_sensor();
size_t unsafe_clear_events();
size_t clear_events(uint64_t timestamp);
std::vector<ForceEvent> get_current_events();
ForceEvent get_last_event();
#endif