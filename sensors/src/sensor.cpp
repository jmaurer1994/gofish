#include "sensor.h"
ADS1115 ADS(0x48);
float peak_force = 0;
float total_force = 0;
int samples_collected = 0;

volatile bool sensor_activity = false;
bool sampling_active = false;

void IRAM_ATTR SAMPLE_START_ISR() {
  // rising edge interrupt
  if(!sampling_active){
    //only alert to activity when not sampling
    sensor_activity = true;
  }
}

void IRAM_ATTR SAMPLE_END_ISR() {
    sensor_activity = false;
}

void sample_force_sensor() {
  if(!sensor_activity && sampling_active){
    sampling_active = false;
    Serial.printf("Sampling over avg: %f peak %f samples %d\n",total_force/samples_collected, peak_force, samples_collected);
    total_force = 0;
    peak_force = 0;
    samples_collected = 0;
    return;
  }

  if(sensor_activity && !sampling_active){
    Serial.println("Started sampling");
    sampling_active = true;
  }

  //do sampling
  float force = ADS.getValue();

  if(force > peak_force){
    peak_force = force;
  }

  total_force += force;
  samples_collected++;
  return;
}

float f = 0;

bool initialize_force_sensor() {
  if(!ADS.isConnected()){
    Serial.println("ADS not connected");
    return false;
  }
  Wire.begin();
  ADS.begin();

  ADS.setGain(1);

  f = ADS.toVoltage();

  ADS.setComparatorMode(0);
  ADS.requestADC(0);

  pinMode(EADC_ALERT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_START_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_END_ISR, FALLING);

  return true;
}

float read_force_sensor(){
    return 0;//analogRead(FORCE_SENSOR_PIN);
}