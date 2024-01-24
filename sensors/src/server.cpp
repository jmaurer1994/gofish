#include "server.h"
#include "conf.h"
#include "scale.h"
#include "sensor.h"
#include "network.h"

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
//TODO: convert to a json
void handleReadScaleRequest(AsyncWebServerRequest *request) {
    if (!request->hasParam("samples")) {
        request->send(400); //no default value available
        return;
    }

    AsyncWebParameter *samplesParam = request->getParam("samples");

    long samples = samplesParam->value().toInt();

    if(!(samples > 0 && samples <= LOADCELL_MAX_SAMPLES_PER_REQUEST)){
        request->send(400); //outside of specified range
        return;
    }
    
    double avg_reading = get_avg_reading(samples);

    String returnval = String(avg_reading, 4);

    request->send(200, "text/plain", returnval);
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
    if (!request->hasParam("timestamp")) {
        request->send(400);
        return;
    }

    uint64_t timestamp = request->getParam("timestamp")->value().toInt();

    if(timestamp == 0){
        request->send(400);
        return;
    }

    size_t count = clear_events(timestamp);

    request->send(200, "text/plain", String(count));
    return;
}

///////////////////////////
// Default
///////////////////////////
void handleRoot(AsyncWebServerRequest *request) {
    request->send(200);
    return;
}

void handleHealthCheck(AsyncWebServerRequest *request) {
    request->send(200);
    return;
}
void handleTimeCheck(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(get_epoch_time()));
    return;
}

void handleNotFound(AsyncWebServerRequest *request) {
    // Handle Unknown Request
    request->send(404);
    return;
}

bool initialize_server() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_OPTIONS, handleHealthCheck);
    server.on("/time", HTTP_GET, handleTimeCheck);

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