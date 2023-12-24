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

void handleRetrieveCurrentEvents(AsyncWebServerRequest *request) {
    std::vector<ForceEvent> current_events = get_current_events();

    DynamicJsonDocument responseObject(WEBSERVER_MEMORYLIMIT);
    JsonArray eventsArray = responseObject.createNestedArray("events");
    
    for (ForceEvent event : current_events) {
        JsonObject eventObject = eventsArray.createNestedObject();
        eventObject["timestamp"] = event.get_timestamp();

        for (float sample : event.get_samples()) {
            eventObject["samples"].add(sample);
        }
    }

    String response;

    if (request->hasParam("pretty")) {
        serializeJsonPretty(responseObject, response);
    } else {
        serializeJson(responseObject, response);
    }

    request->send(200, "application/json", response);
    return;
}

void handleClearCurrentEvents(AsyncWebServerRequest *request) {
    unsafe_clear_events();

    request->send(200);
    return;
}

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

    server.on("/force/events/read_current", HTTP_GET,
              handleRetrieveCurrentEvents);
    server.on("/force/events/clear_current", HTTP_GET,
              handleClearCurrentEvents);

    server.onNotFound(handleNotFound);

    server.begin();
    DEBUG_PRINTF1("Server listening on port %d\n", WEBSERVER_PORT);
    return true;
}