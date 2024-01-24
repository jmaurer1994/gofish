#ifndef DB_H
#define DB_H
#include "sensor.h"

#include <AsyncHTTPRequest_Generic.hpp>
bool initialize_db_connection();
bool sendSensorEventInsertRequest(ForceEvent event);
#endif