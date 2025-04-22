#define MQTT_VERSION MQTT_VERSION_3_1_1

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <iot_connect.h>

#define LED_R 32
#define LED_B 33

#define WIFI_SSID "TEAM IT"
#define WIFI_PASS "1TM1TR4101101MIVHS2025"

IoTConnect iot_connect("fPzV-eU0s-voSv-hhXv-KcFG-ZOuQ-r-", "bOilVfLrIl");

void handle_led_red(String value) {
    Serial.print("[FROM ESP32] Value: ");
    Serial.println(value);

    digitalWrite(LED_R, ((value == "1") ? HIGH : LOW));
}

void handle_led_blue(String value) {
    Serial.print("[FROM ESP32] Value: ");
    Serial.println(value);

    digitalWrite(LED_B, ((value == "1") ? HIGH : LOW));
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
    
    digitalWrite(LED_R, 1);
    digitalWrite(LED_B, 1);
    delay(5000);
    digitalWrite(LED_R, 0);
    digitalWrite(LED_B, 0);

    iot_connect.bind_controllable(handle_led_red, "LED 1");
    iot_connect.bind_controllable(handle_led_blue, "LED 2");
}

void loop() {
    iot_connect.loop();
}
