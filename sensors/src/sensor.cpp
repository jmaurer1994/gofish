#include "sensor.h"
#include "conf.h"
ADS1115 ADS(0x48);

volatile bool sensor_activity = false;
volatile bool sampling_active = false;

void IRAM_ATTR SAMPLE_START_ISR() {
    if (!sampling_active) {
        sensor_activity = true;
    }
}

std::vector<ForceEvent> current_events;
ForceEvent current_event;

bool sample_force_sensor() {
    static float last_reading = 0;
    
    if (!sensor_activity && !sampling_active) {
        return false; // no sensor activity & not sampling
    }

    if (sensor_activity && !sampling_active) {
        sampling_active = true;
        current_event.init_object(); // set timestamp
        DEBUG_PRINTF1("Started sampling: %d\n", current_event.get_timestamp());
    }

    // do sampling
    float force = ADS.getValue();

    if (force < 100) {
        // stop sampling
        if (current_event.samples_collected() > 0) {
            DEBUG_PRINTF3("Recorded event:\navg:%f\tpeak:%f\tsamples:%d\n",
                          current_event.average_force(),
                          current_event.peak_force(),
                          current_event.samples_collected());
            current_events.push_back(current_event);
            current_event.reset_object(); // clear list
        } // don't save phantom events (happens on wifi dc?)

        sensor_activity = false; // reset int
        sampling_active = false;
        return false;
    }

    if (force != last_reading) {
        current_event.record_sample(force);
    }

    last_reading = force;

    return true;
}

bool initialize_force_sensor() {
    Wire.begin();
    ADS.begin();

    ADS.setMode(0);
    ADS.setGain(1);

    float f = ADS.toVoltage(1);
    DEBUG_PRINTLN("Setting up comparator");

    ADS.setComparatorMode(0);  // TRADITIONAL
    ADS.setComparatorLatch(0); // NON-LATCH

    ADS.setComparatorThresholdHigh(EADC_COMPARATOR_THRESHOLD_HIGH);
    ADS.setComparatorThresholdLow(EADC_COMPARATOR_THRESHOLD_LOW);

    ADS.setComparatorQueConvert(0); // Trigger after 1 conversion?

    ADS.requestADC(0);
    DEBUG_PRINTLN("Intialized external ADC"); // Sends above settings to ADC

    pinMode(EADC_ALERT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_START_ISR,
                    RISING);

    if (!ADS.isConnected()) {
        DEBUG_PRINTLN("ADS not connected!");
    }

    return true;
}

std::vector<ForceEvent> get_current_events() { return current_events; }

size_t clear_events(uint64_t timestamp) {
    uint64_t currentTime = get_epoch_time();

    size_t event_count = current_events.size();

    current_events.erase(
        std::remove_if(current_events.begin(), current_events.end(),
                       [currentTime](const ForceEvent &event) {
                           return event.get_timestamp() <= currentTime;
                       }),
        current_events.end());

    return event_count - current_events.size();
}

size_t unsafe_clear_events() {
    current_events.clear();
    return current_events.size();
}