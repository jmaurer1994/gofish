#ifndef SERVER_H
#define SERVER_H

#include "conf.h"
#include "scale.h"
#include "sensor.h"

#include <ESPAsyncWebSrv.h>
#include <AsyncJson.h>

bool initialize_server();
#endif