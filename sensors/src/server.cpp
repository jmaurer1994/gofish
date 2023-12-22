#include "server.h"

AsyncWebServer server(WEBSERVER_PORT);

/******************************************************************
 * Route Handlers
 *
 *
 *
 */

///////////////////////////
// Scale
///////////////////////////
void handleTareScaleRequest(AsyncWebServerRequest *request) {
    tare_scale_by_value();
    request->send(200);
}

void handleTareResetScaleRequest(AsyncWebServerRequest *request) {
    reset_tare_value();
    request->send(200);
}

void handleReadScaleRequest(AsyncWebServerRequest *request) {
    if (request->hasParam("readings")) {
        AsyncWebParameter *readingsParam = request->getParam("readings");

        long readings = readingsParam->value().toInt();

        if (readings > 0 && readings <= 100) {
            double avg_reading = get_avg_reading(readings);

            String returnval = String(avg_reading, 4);

            request->send(200, "text/plain", returnval);
            return;
        }
    }
    request->send(402);
    return;
}

///////////////////////////
// Force Sensor
///////////////////////////

///////////////////////////
// Default
///////////////////////////
void handleHealthCheck(AsyncWebServerRequest *request) {
    request->send(200);
    return;
}

void handleNotFound(AsyncWebServerRequest *request) {
    // Handle Unknown Request
    request->send(404);
    return;
}

bool initialize_server() {
    server.on("/", HTTP_GET, handleHealthCheck);
    server.on("/scale/tare/reset", HTTP_GET, handleTareResetScaleRequest);
    server.on("/scale/tare", HTTP_GET, handleTareScaleRequest);
    server.on("/scale/read", HTTP_GET, handleReadScaleRequest);

    server.onNotFound(handleNotFound);

    server.begin();
    return true;
}