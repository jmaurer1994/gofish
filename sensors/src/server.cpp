#include "server.h"

AsyncWebServer server(80);

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
    } else {
      request->send(402);
    }
  }
}

///////////////////////////
// Force Sensor
///////////////////////////

void handleSensorReadRequest(AsyncWebServerRequest *request) {
    String value;
    value = String(read_force_sensor());
    request->send(200, "application/json", value);
}

///////////////////////////
// Default
///////////////////////////
void handleNotFound(AsyncWebServerRequest *request) {
  // Handle Unknown Request
  request->send(404);
}

void initialize_server() {
  server.on("/scale/tare/reset", HTTP_GET, handleTareResetScaleRequest);
  server.on("/scale/tare", HTTP_GET, handleTareScaleRequest);
  server.on("/scale/read", HTTP_GET, handleReadScaleRequest);
  server.on("/sensor/read", HTTP_GET, handleSensorReadRequest);

  server.onNotFound(handleNotFound);

  server.begin();
}