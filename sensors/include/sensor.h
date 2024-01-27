#ifndef SENSOR_H
#define SENSOR_H

#include <ADS1X15.h>
#include <vector>

class ForceEvent {
  uint64_t timestamp;
  std::vector<float> samples;

public:
  void record_sample(float value);
  void init_object();
  void reset_object();
  uint64_t get_timestamp() const;
  std::vector<float> get_samples();
  uint16_t samples_collected();
  float peak_force();
  float average_force();
};

bool initialize_force_sensor();
bool sample_force_sensor();
size_t unsafe_clear_events();
size_t clear_events_range(uint64_t timestamp);
std::vector<ForceEvent> get_current_events();
ForceEvent get_event(uint64_t timestamp);
ForceEvent get_last_event();

#endif