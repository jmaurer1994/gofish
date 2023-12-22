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
    if (!sensor_activity && !sampling_active) {
        return false; // no sensor activity & not sampling
    }

    if (sensor_activity && !sampling_active) {
        DEBUG_PRINTLN("Started sampling");
        sampling_active = true;
        current_event.init_object(); // set timestamp
    }

    // do sampling
    float force = ADS.getValue();

    if (force < 100) {
        // stop sampling
        if (current_event.samples_collected() > 0) {
            current_events.push_back(current_event);
            current_event.reset_object(); // clear list
        } // don't save phantom events (happens on wifi dc?)

        sensor_activity = false; // reset int
        sampling_active = false;
        return false;
    }

    current_event.record_sample(force);

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

    ADS.setComparatorThresholdHigh(0x0250);
    ADS.setComparatorThresholdLow(0x0100);

    ADS.setComparatorQueConvert(0); // Trigger after 1 conversion?

    ADS.requestADC(0);
    DEBUG_PRINTLN("Intialized ADC"); // Sends above settings to ADC

    pinMode(EADC_ALERT_PIN, INPUT_PULLUP);
    DEBUG_PRINTLN("Configured alert pin");

    attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_START_ISR,
                    RISING);

    DEBUG_PRINTLN("Attached interrupt");

    if (!ADS.isConnected()) {
        DEBUG_PRINTLN("ADS not connected!");
    }

    return true;
}

void persist_events() {
    // saves events to NAND
}