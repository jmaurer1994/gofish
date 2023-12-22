#ifndef WIFI_H
#define WIFI_H

/**
 * dependencies
 */
#include <WiFi.h>
#include <WString.h>

#include "conf.h"

bool initialize_wifi();
void check_wifi_status();

#endif