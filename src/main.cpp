#define MQTT_VERSION MQTT_VERSION_3_1_1

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define LED_R 32
#define LED_B 33

#define WIFI_SSID "TEAM IT"
#define WIFI_PASS "1TM1TR4101101MIVHS2025"

#define MQTT_SERVER_URL "192.168.9.134"
#define MQTT_SERVER_PORT 1883
#define MQTT_USER "iwan"
#define MQTT_PASS "Iwan123"

WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);

void reconnect_to_mqtt() {
    while (!mqtt_client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqtt_client.connect("ESP32Client", MQTT_USER, MQTT_PASS)) {
            Serial.println("connected");

            // Subscribe to a topic
            mqtt_client.subscribe("esp32/ledr");
            mqtt_client.subscribe("esp32/ledb");
            mqtt_client.publish("test/topic", "Hello, From ESP32!!");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" trying again in 5 seconds");
            delay(5000);
        }
    }
}
  

void handle_mqtt_data(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");

    String value = "";
    for (int i = 0; i < length; i++) {
        value += ((char)payload[i]);
    }
    
    Serial.println(value);

    if(String(topic).startsWith("esp32/ledr")) {
        Serial.print("LED R is ");
        Serial.println((value == "1" ? "ON" : "OFF"));
        digitalWrite(LED_R, (value == "1" ? 1 : 0));
        delay(1000);
    }
    else if(String(topic).startsWith("esp32/ledb")) {
        Serial.print("LED B is ");
        Serial.println((value == "1" ? "ON" : "OFF"));
        digitalWrite(LED_B, (value == "1" ? 1 : 0));
        delay(1000);
    }
    
    Serial.println();
}

void setup() {
    Serial.begin(9600);

    pinMode(LED_R, OUTPUT);
    pinMode(LED_B, OUTPUT);
    
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("Connecting to WiFi");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi Conencted!");
    Serial.print("IP :");
    Serial.println(WiFi.localIP());

    Serial.print("Setup MQTT Connection to ");
    Serial.print(MQTT_SERVER_URL);
    Serial.print(":");
    Serial.println(MQTT_SERVER_PORT);

    mqtt_client.setServer(MQTT_SERVER_URL, MQTT_SERVER_PORT);
    mqtt_client.setCallback(handle_mqtt_data);
    
    digitalWrite(LED_R, 1);
    digitalWrite(LED_B, 1);
    delay(5000);
    digitalWrite(LED_R, 0);
    digitalWrite(LED_B, 0);
    
    Serial.println("Sending an MQTT Subscribe Request");
}

void loop() {
    if(!mqtt_client.connected()) {
        reconnect_to_mqtt();
    }
    mqtt_client.loop();
}
