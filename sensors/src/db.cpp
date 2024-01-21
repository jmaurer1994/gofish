#include "db.h"
AsyncHTTPRequest authRequest;
AsyncHTTPRequest insertRequest;
String jwtToken;

boolean waitingRequest = false;



bool sendAuthenticationRequest();
bool sendSensorEventInsertRequest();

void authRequestCB(void *optParm, AsyncHTTPRequest *request, int readyState) {
  (void)optParm;

  if (readyState == readyStateDone) {
    DEBUG_PRINTF1("Response Code = %s", request->responseHTTPString());

    if (request->responseHTTPcode() == 200) {
      DynamicJsonDocument responseObject(512);
      deserializeJson(responseObject, request->responseText());

      jwtToken = responseObject["token"].as<String>();
      DEBUG_PRINTF1("TOKEN: %s", jwtToken);
      if(waitingRequest){
        waitingRequest = false;
        sendSensorEventInsertRequest(); 
      }
    }
  }
}

void insertRequestCB(void *optParm, AsyncHTTPRequest *request, int readyState) {
  (void)optParm;

  if (readyState == readyStateDone) {
    DEBUG_PRINTF1("Response Code = %s", request->responseHTTPString());

    if (request->responseHTTPcode() == 200) {
      DEBUG_PRINTLN(request->responseText());
    }
    if (request->responseHTTPcode() == 401) {
      DEBUG_PRINTLN("Request not authenticated");
      waitingRequest = true;
      sendAuthenticationRequest();
    }
  }
}

bool initialize_db_connection() {
  authRequest.onReadyStateChange(authRequestCB);
  insertRequest.onReadyStateChange(insertRequestCB);

  // get an auth token
  sendAuthenticationRequest();
  return true;
}

bool sendAuthenticationRequest() {
  static bool requestOpenResult;

  if (!(authRequest.readyState() == readyStateUnsent ||
        authRequest.readyState() == readyStateDone)) {
    return false; // not ready
  }

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


bool sendSensorEventInsertRequest() {
  static bool requestOpenResult;

  if (!(insertRequest.readyState() == readyStateUnsent ||
        insertRequest.readyState() == readyStateDone)) {
    return false; // not ready
  }

  requestOpenResult = insertRequest.open("POST", DB_DATA_URL);
  if (!requestOpenResult) {
    // Only send() if open() returns true, or crash
    DEBUG_PRINTLN("Can't send bad request");
    return false;
  }
  insertRequest.setReqHeader("Authorization", ("Bearer " + jwtToken).c_str());
  insertRequest.setReqHeader("Content-Profile", "data");

  // send body
  DynamicJsonDocument requestObject(4096);

  ForceEvent event = get_last_event();

  requestObject["timestamp"] = event.get_timestamp();

  for (float sample : event.get_samples()) {
    requestObject["samples"].add(sample);
  }

  String request;

  serializeJson(requestObject,request);

  insertRequest.send(request);

  DEBUG_PRINTLN("Insert request sent");
  return true;
}