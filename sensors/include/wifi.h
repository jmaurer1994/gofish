#ifndef WIFI_H
#define WIFI_H

/**
 * dependencies
 */
#include "lib.h"

#include <WiFi.h>

enum WiFiModuleState {
  WIFI_UNINITIALIZED,
  WIFI_CONNECTED,
  WIFI_RECONNECTING,
  WIFI_FAILED,
  NUM_WIFIMODULESTATES,
};

enum WiFiStateReason {
  WIFI_OK,
  WIFI_CONFIGURATION_INVALID,
  WIFI_CONFIGURATION_FAILED,
  NUM_WIFIMODULESTATEREASONS,
};

bool initialize_wifi();
void check_wifi_status();

#endif