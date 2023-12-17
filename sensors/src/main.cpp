
#include "lib.h"
#include "scale.h"
#include "sensor.h"
#include "server.h"
#include "wifi.h"

void setup()
{
  Serial.begin(115200);
  initialize_wifi();
  //initialize_scale();
  initialize_server();
  initialize_force_sensor();
}

void loop()
{
  check_wifi_status();
}