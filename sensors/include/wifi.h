#ifndef WIFI_H
#define WIFI_H

/**
 * dependencies
 */
#include "conf.h"

#include <WString.h>
#include <WiFi.h>

bool initialize_wifi();
void check_wifi_status();

#endif