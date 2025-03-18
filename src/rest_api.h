#ifndef REST_API_H
#define REST_API_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

void setupRestApi();
void handleGetHealth();

// Helper functions to send error responses
String sendInvalidRequestBodyResponse();
String sendInvalidJsonResponse();

// Helper function to generate a structured response
String sendResponse(int responseCode, JSONVar responseDoc);


#endif
