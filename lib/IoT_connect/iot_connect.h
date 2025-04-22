#pragma once

#include <HTTPClient.h>
#include <WiFi.h>
#include <additionals.h>
#include <PubSubClient.h>
#include <string.h>
#include <vector>

struct topic_data {
    String topic_name;
    String controllable_name;
    void (*callback)(String value);
};

WiFiClient esp32_client;
PubSubClient mqtt_client(esp32_client);

const String API_BASE_ENDPOINT = "http://192.168.9.134:3000";
const String DEVICE_INITIALIZATION_URL = API_BASE_ENDPOINT+"/api/device/initialize";
const String DEVICE_CONNECT_CONTROLLABLE_URL = API_BASE_ENDPOINT+"/api/device/connect_controllable";

const char* MQTT_SERVER_URL = "192.168.9.134";
const uint16_t MQTT_SERVER_PORT = 1883;

//ANCHOR - Class
class IoTConnect
{
private:
    String DEVICE_KEY;
    String DEVICE_PASS;

    String mqtt_user;
    String mqtt_pass;

    std::vector<topic_data> subscribed_topics;

    // Log for debugging specific to IOT CONNECT
    void debug_log(String text);
    void setup_http_client(HTTPClient *http_client, String url);

    bool check_connection(void);
    void mqtt_handler(char* topic, byte* payload, unsigned int length);
    
public:
    IoTConnect(String UUID, String PASSKEY);
    void connect(void);
    void loop(void);
    void bind_controllable(void (*callback)(String value), String controllable_name);
    
    bool connected = false;

    static IoTConnect* instance;
    static void static_mqtt_handler(char* topic, byte* payload, unsigned int length) {
        if(instance != nullptr) {
            instance->mqtt_handler(topic, payload, length);
        }
    }
};


void IoTConnect::debug_log(String text) {
    Serial.println("[IOT CONNECT] "+String(text));
}


//SECTION - Functions
IoTConnect::IoTConnect(String device_key, String device_pass)
{
    instance = this;
    
    this->DEVICE_KEY = device_key;
    this->DEVICE_PASS = device_pass;
}


// ### Initial Connection
// Connecting Device to the IoT Connect Service
void IoTConnect::connect() {
    // Check if the ESP connected to a WiFi or not.
    mqtt_client.setServer(MQTT_SERVER_URL, MQTT_SERVER_PORT);
    mqtt_client.setKeepAlive(60);
    
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http_client;
        http_client.begin(DEVICE_INITIALIZATION_URL);
        http_client.addHeader("Content-Type", "application/json");
        http_client.addHeader("authorization", "Ghanw@n13579@691000$");
        int http_response_code = http_client.POST("{ \"device_key\": \""+String(this->DEVICE_KEY)+"\", \"device_pass\": \""+String(this->DEVICE_PASS)+"\" }"); // Send POST request
    
        if (http_response_code <= 0) {
            this->debug_log("Error when trying to connect to API");
            return;
        }

        this->debug_log("Successfully connect to API!");
        
        http_client.end(); // Free resources
    } 
    else {
        this->debug_log("WiFi Disconnected...");
    }
}

// ### Check Connection
// This function is used to check the connection between ESP32 and MQTT server
bool IoTConnect::check_connection() {
    while (!mqtt_client.connected()) {
        if(this->connected) {
            this->debug_log("Reconnecting");
            this->connected = false;
        }
        
        mqtt_client.connect("ESP32", "iwan", "Iwan123");

        delay(1000);
    }

    if(!this->connected) {
        this->debug_log("Connected!");

        for(uint8_t i = 0; i < this->subscribed_topics.size(); i++) {
            mqtt_client.subscribe(this->subscribed_topics[i].topic_name.c_str());
        }

        mqtt_client.setCallback(IoTConnect::static_mqtt_handler);
        this->connected = true;
    }

    return true;
}

// ### Main Loop
// This function must be called once under `void loop()`
void IoTConnect::loop() {
    if(this->subscribed_topics.size() > 0) {
        this->check_connection();
    }
    mqtt_client.loop();
}

void IoTConnect::mqtt_handler(char* topic, byte* payload, unsigned int length) {
    String msg = "";

    for (uint32_t i = 0; i < length; i++) {
        msg += (char)payload[i];
    }

    for(uint8_t i = 0; i < this->subscribed_topics.size(); i++) {
        if(subscribed_topics[i].topic_name == topic) {
            subscribed_topics[i].callback(msg);
        }
    }
}

// ### Bind Controllable
// This function bind controllable to a function
void IoTConnect::bind_controllable(void (*callback)(String value), String controllable_name) {
    HTTPClient http_client;
    this->setup_http_client(&http_client, DEVICE_CONNECT_CONTROLLABLE_URL);
    
    int http_response_code = 0;
    while(http_response_code <= 0) {
        http_response_code = http_client.POST("{\"controllable_name\": \""+controllable_name+"\", \"device_key\":\""+this->DEVICE_KEY+"\", \"device_pass\": \""+this->DEVICE_PASS+"\"}");
    
        if (http_response_code <= 0) {
            this->debug_log("Error when trying to connect to API");
            delay(5000);
        }
    }
    
    String response_data[3];
    
    // Trying to get 3 data from response value
    {
        String response = http_client.getString();
        http_client.end();
        
        char response_array[200];
        response.toCharArray(response_array, sizeof(response_array));
        char* token = strtok(response_array, ",");
    
        uint8_t index = 0;
        while (token != NULL) {
            response_data[index++] = token;
            token = strtok(NULL, ",");
        }
    }

    this->mqtt_user = response_data[1];
    this->mqtt_pass = response_data[2];

    // Connect to the mqtt first if this is the first time
    if(this->subscribed_topics.size() == 0) {
        this->debug_log("Connecting to MQTT Server..");
        this->check_connection();
    }

    mqtt_client.subscribe(response_data[0].c_str());
    this->debug_log("Successfully Connect to : ["+controllable_name+"]");

    this->subscribed_topics.push_back({
        response_data[0],
        controllable_name,
        callback
    });
}

void IoTConnect::setup_http_client(HTTPClient *http_client, String url) {
    http_client->begin(url);
    http_client->addHeader("Content-Type", "application/json");
    http_client->addHeader("authorization", "IA9as9dASya9fashi12raCasdwq");
}

IoTConnect* IoTConnect::instance = nullptr;
//!SECTION