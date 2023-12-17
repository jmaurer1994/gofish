#include "sensor.h"
ADS1115 ADS(0x48);
float peak_force = 0;
float total_force = 0;
double peak_force_timestamp = 0;

//TODO: rework this struct to improve structure of file/store more data
struct ForceReading {
  float value = 0;
  double timestamp = 0;
  int sensor_reads = 0;
};

ForceReading saved_events[FORCE_SENSOR_MAX_EVENTS];
int events_recorded = 0;

volatile bool sensor_activity = false;
bool sampling_active = false;
int samples_remaining = 0;

volatile bool pin_low_encountered = false;
volatile bool pin_high_encountered = false;

void IRAM_ATTR SAMPLE_START_ISR() {
  // rising edge interrupt
  if(!sampling_active){
    //only alert to activity when not sampling
    sensor_activity = true;
  }
}

void IRAM_ATTR SAMPLE_END_ISR() {
  if(sampling_active){
    sensor_activity = false;
  }
}

void IRAM_ATTR PIN_HIGH_ISR() {
  if(!pin_high_encountered){
    pin_high_encountered = true;
  }
}
void IRAM_ATTR PIN_LOW_ISR() {
  if(!pin_low_encountered){
    pin_low_encountered = true;
  }
}

void initialize_force_sensor() {
  pinMode(EADC_ALERT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_START_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_END_ISR, FALLING);

  attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), PIN_HIGH_ISR, HIGH);
  attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), PIN_LOW_ISR, LOW);
}

float read_force_sensor(){
    return 0;//analogRead(FORCE_SENSOR_PIN);
}