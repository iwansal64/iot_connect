#pragma once

#include <HTTPClient.h>
#include <WiFi.h>
#include <additionals.h>
#include <ArduinoJson.h>

const String DEVICE_INITIALIZATION_URL = "";

//ANCHOR - Class
class iot_connect
{
private:
    char* UUID;

    // Log for debugging specific to IOT CONNECT
    void debug_log(String text) {
        Serial.println("[IOT CONNECT] "+String(text));
    }
    
public:
    iot_connect(char* UUID);
    void connect( );
};




//SECTION - Functions
iot_connect::iot_connect(char* UUID)
{
    this->UUID = UUID;
}

//ANCHOR - Connecting to MQTT
void iot_connect::connect() {
    // Check if the ESP connected to a WiFi or not.
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(DEVICE_INITIALIZATION_URL);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST("{ \"device_key\": \""+String(this->UUID)+"\" }"); // Send POST request
    
        if (httpResponseCode <= 0) {
            this->debug_log("Error when trying to connect to API");
            return;
        }
        
        String response = http.getString();
        
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);
        
        if(error) {
            
            return;
        }
        
    
        http.end(); // Free resources
    } 
    else {
        this->debug_log("WiFi Disconnected...");
    }
}

//!SECTION