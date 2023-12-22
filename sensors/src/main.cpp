
#include "main.h"

void setup() {

    DEBUG_BEGIN(SERIAL_BAUD_RATE);

    delay(INITIAL_STARTUP_DELAY);
    DEBUG_PRINTLN("Starting up");

    if (!initialize_wifi()) {
        DEBUG_PRINTLN("Failed to initialize wifi");
    }

    if (!initialize_server()) {
        DEBUG_PRINTLN("Failed to initialize web server");
    }

    if (!initialize_force_sensor()) {
        DEBUG_PRINTLN("Failed to initialize force sensor");
    }

    if (!initialize_scale()) {
        DEBUG_PRINTLN("Failed to initialize scale");
    }
    
    DEBUG_PRINTLN("Done setup");
}

void loop() {
    if (!sample_force_sensor()) {
        check_wifi_status();
    }
}
