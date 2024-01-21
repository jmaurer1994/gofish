#ifndef WIFI_H
#define WIFI_H

/**
 * dependencies
 */
#include "conf.h"

#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <WiFi.h>

bool initialize_network();
void run_network_checks();

uint64_t get_epoch_time();

#endif