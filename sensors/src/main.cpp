
#include "lib.h"
#include "scale.h"
#include "sensor.h"
#include "server.h"
#include "wifi.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting up");
    if (!initialize_wifi()) {
        Serial.println("Failed to initialize wifi");
  } else {
    Serial.println("Initialized WiFi module");
  }
  if (!initialize_server()) {
      Serial.println("Failed to initialize web server");
  } else {
      Serial.println("Initialized web server");
  }
  if (!initialize_force_sensor()) {
      Serial.println("Failed to initialize force sensor");
  } else {
      Serial.println("Force sensor initialized");
  }

  // initialize_scale();
  
}

void loop()
{
  check_wifi_status();
  sample_force_sensor();
}