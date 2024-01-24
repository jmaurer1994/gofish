#include "db.h"
#include "conf.h"

AsyncHTTPRequest authRequest;
AsyncHTTPRequest insertRequest;
String jwtToken;

std::vector<ForceEvent> sentEvents;
std::vector<ForceEvent> unsentEvents;
static bool waitingAuthRequest;

bool sendAuthenticationRequest();
void resend_event();
uint8_t confirm_event_entry(uint64_t timestamp);


#define ASYNC_HTTP_DEBUG_PORT     Serial
#define _ASYNC_HTTP_LOGLEVEL_     4

void authRequestCB(void *optParm, AsyncHTTPRequest *request, int readyState) {
  (void)optParm;

  if (readyState == readyStateDone) {
    DEBUG_PRINTF1("Response Code = %s", request->responseHTTPString());

    if (request->responseHTTPcode() == 200) {
      DynamicJsonDocument responseObject(512);
      deserializeJson(responseObject, request->responseText());
      DEBUG_PRINTLN(request->responseText());
      jwtToken = responseObject["token"].as<String>();
      DEBUG_PRINTF1("TOKEN: %s", jwtToken);
      waitingAuthRequest = false;
      resend_event();
    }
  }
}

void insertRequestCB(void *optParm, AsyncHTTPRequest *request, int readyState) {
  (void)optParm;

  if (readyState == readyStateDone) {
    DEBUG_PRINTF1("Response Code = %s\n", request->responseHTTPString());
    DEBUG_PRINTF1("%s\n", request->headers());
    if (request->responseHTTPcode() == 200) {
      // get timestamp, remove from sent requests
      DEBUG_PRINTLN(request->responseText());
      DynamicJsonDocument responseObject(512);
      deserializeJson(responseObject, request->responseText());
      uint64_t timestamp = responseObject["timestamp"];
      confirm_event_entry(timestamp);
    }
    if (request->responseHTTPcode() == 401) {
      DEBUG_PRINTLN("Request not authenticated");
      if (!waitingAuthRequest) {
        sendAuthenticationRequest();
      }
    }
  }
}
uint8_t confirm_event_entry(uint64_t timestamp) {
  uint8_t event_count = sentEvents.size();
  sentEvents.erase(std::remove_if(sentEvents.begin(), sentEvents.end(),
                                  [timestamp](const ForceEvent &event) {
                                    return event.get_timestamp() == timestamp;
                                  }),
                   sentEvents.end());
  return event_count - sentEvents.size();
}

void resend_event() {
  if (!waitingAuthRequest) {
    return;
  }

  if (sentEvents.size() > 0) {
    ForceEvent event = sentEvents.back();
    sentEvents.pop_back();
    sendSensorEventInsertRequest(event);
    return;
  }

  if (unsentEvents.size() > 0) {
    ForceEvent event = unsentEvents.back();
    unsentEvents.pop_back();
    sendSensorEventInsertRequest(event);
    return;
  }

  return;
}
bool initialize_db_connection() {
  authRequest.onReadyStateChange(authRequestCB);
  insertRequest.onReadyStateChange(insertRequestCB);

  // get an auth token
  return true;
}

bool sendAuthenticationRequest() {
  static bool requestOpenResult;

  if (!(authRequest.readyState() == readyStateUnsent ||
        authRequest.readyState() == readyStateDone)) {
    return false; // not ready
  }

  DEBUG_PRINTLN("SENDING AUTH REQ");
  authRequest.setDebug(true);
  requestOpenResult = authRequest.open("POST", DB_LOGIN_URL);
  if (!requestOpenResult) {
    // Only send() if open() returns true, or crash
    DEBUG_PRINTLN("Can't send bad request");
    return false;
  }

  // send body
  DynamicJsonDocument requestObject(512);

  requestObject["username"] = DB_USER;
  requestObject["password"] = DB_PASSWORD;

  String request;

  serializeJson(requestObject, request);

  authRequest.send(request);

  DEBUG_PRINTLN("Auth request sent");
  return true;
}

bool sendSensorEventInsertRequest(ForceEvent event) {
  static bool requestOpenResult;
  if (jwtToken == "") {
    unsentEvents.push_back(event);
    sendAuthenticationRequest();
    return false;
  }

  if (!(insertRequest.readyState() == readyStateUnsent ||
        insertRequest.readyState() == readyStateDone)) {
    unsentEvents.push_back(event);
    return false; // not ready
  }
  DEBUG_PRINTLN("SENDING INSERT REQ");
  insertRequest.setDebug(true);
  requestOpenResult = insertRequest.open("POST", DB_DATA_URL);

  if (!requestOpenResult) {
    // Only send() if open() returns true, or crash
    unsentEvents.push_back(event);
    DEBUG_PRINTLN("Can't send bad request");
    return false;
  }

  insertRequest.setReqHeader("Authorization", ("Bearer " + jwtToken).c_str());
  insertRequest.setReqHeader("Content-Profile", "data");
  insertRequest.setReqHeader("Prefer", "return=representation");

  // send body
  DynamicJsonDocument requestObject(4096);

  requestObject["timestamp"] = event.get_timestamp();

  for (float sample : event.get_samples()) {
    requestObject["samples"].add(sample);
  }

  String request;
  serializeJson(requestObject, request);

  insertRequest.send(request);
  sentEvents.push_back(event);
  DEBUG_PRINTLN(request);
  DEBUG_PRINTLN("Insert request sent");
  return true;
}