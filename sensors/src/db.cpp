#include "db.h"
#include "conf.h"

AsyncHTTPRequest authRequest;
AsyncHTTPRequest insertRequest;
std::vector<ForceEvent> sentEvents;
std::vector<ForceEvent> unsentEvents;
static bool waitingAuthRequest;

DynamicJsonDocument authResponseObject(512); // holds current JWT value
DynamicJsonDocument insertResponseObject(
    1024); // receives timestamp from inserted event for event cleanup
DynamicJsonDocument
    insertRequestObject(4096); // facilitates sending of event for insertion

DynamicJsonDocument authRequestObject(512);
bool sendAuthenticationRequest();
bool resend_queued_event();
uint8_t confirm_event_entry(uint64_t timestamp);
void auth_request_cb(void *optParm, AsyncHTTPRequest *request, int readyState) {
  (void)optParm;
  if (readyState != readyStateDone) {
    return;
  }
  DEBUG_PRINTF2("AUTH REQUEST RESPONSE %d = %s\n", request->responseHTTPcode(),
                request->responseHTTPString());

  if (request->responseHTTPcode() == 200) {
    const String response = request->responseText();
    DeserializationError err = deserializeJson(authResponseObject, response);
    if (err) {
      DEBUG_PRINTF1("JSON DESERIALIZATION ERROR: %s\n", err.c_str());
    }
    waitingAuthRequest = false;
    if (resend_queued_event()) {
      DEBUG_PRINTLN("RESENT QUEUED EVENT");
    };
  }
}

void insert_request_cb(void *optParm, AsyncHTTPRequest *request,
                       int readyState) {
  (void)optParm;

  if (readyState != readyStateDone) {
    return;
  }

  DEBUG_PRINTF2("INSERT REQUEST RESPONSE %d = %s\n",
                request->responseHTTPcode(), request->responseHTTPString());
  if (request->responseHTTPcode() == 201) {
    const String response = request->responseText();

    DeserializationError err = deserializeJson(insertResponseObject, response);
    if (err) {
      DEBUG_PRINTF1("JSON DESERIALIZATION ERROR: %s\n", err.c_str());
    }

    uint64_t timestamp = insertResponseObject[0]["timestamp"];

    printf("%d", timestamp);
    if (!confirm_event_entry(timestamp)) {
      DEBUG_PRINTF1("EVENT %llu NOT CONFIRMED", timestamp);
    }
    return;
  }

  if (request->responseHTTPcode() == 401) {
    DEBUG_PRINTLN("REQUEST NOT AUTHENTICATED");
    if (!waitingAuthRequest) {
      sendAuthenticationRequest();
      DEBUG_PRINTLN("SENT AUTH REQUEST");
    }
    return;
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

boolean resend_queued_event() {
  if (waitingAuthRequest) {
    return false;
  }

  if (sentEvents.size() > 0) {
    ForceEvent event = sentEvents.back();
    sentEvents.pop_back();
    sendSensorEventInsertRequest(event);
    return true;
  }

  if (unsentEvents.size() > 0) {
    ForceEvent event = unsentEvents.back();
    unsentEvents.pop_back();
    sendSensorEventInsertRequest(event);
    return true;
  }
  return false;
}

bool initialize_db_connection() {
  authRequest.onReadyStateChange(auth_request_cb);
  insertRequest.onReadyStateChange(insert_request_cb);

  // get an auth token
  return true;
}

bool sendAuthenticationRequest() {
  static bool requestOpenResult;

  if (!(authRequest.readyState() == readyStateUnsent ||
        authRequest.readyState() == readyStateDone)) {
    return false; // not ready
  }

  authRequest.setDebug(false);
  requestOpenResult = authRequest.open("POST", DB_LOGIN_URL);
  if (!requestOpenResult) {
    // Only send() if open() returns true, or crash
    DEBUG_PRINTLN("Can't send bad request");
    return false;
  }

  // send body

  authRequestObject["username"] = DB_USER;
  authRequestObject["password"] = DB_PASSWORD;

  String request;

  serializeJson(authRequestObject, request);

  authRequest.send(request);

  return true;
}

bool sendSensorEventInsertRequest(ForceEvent event) {

  static bool requestOpenResult;
  if (!authResponseObject["token"]) {
    DEBUG_PRINTLN("NEED AUTHENTICATION BEFORE INSERTION");
    unsentEvents.push_back(event);
    sendAuthenticationRequest();
    return false;
  }

  if (!(insertRequest.readyState() == readyStateUnsent ||
        insertRequest.readyState() == readyStateDone)) {
    DEBUG_PRINTLN("INSERT REQUEST NOT READY");
    unsentEvents.push_back(event);
    return false; // not ready
  }
  insertRequest.setDebug(false);
  requestOpenResult = insertRequest.open("POST", DB_DATA_URL);

  if (!requestOpenResult) {
    // Only send() if open() returns true, or crash
    unsentEvents.push_back(event);
    DEBUG_PRINTLN("Can't send bad request");
    return false;
  }

  insertRequest.setReqHeader(
      "Authorization",
      ("Bearer " + authResponseObject["token"].as<String>()).c_str());
  insertRequest.setReqHeader("Content-Type", "application/json");
  insertRequest.setReqHeader("Content-Profile", "data");
  insertRequest.setReqHeader("Prefer", "return=representation");

  // send body

  insertRequestObject["timestamp"] = event.get_timestamp();

  for (float sample : event.get_samples()) {
    insertRequestObject["samples"].add(sample);
  }

  String request;
  serializeJson(insertRequestObject, request);

  insertRequest.send(request);
  sentEvents.push_back(event);
  return true;
}