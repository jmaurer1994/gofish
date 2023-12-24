#include "wifi.h"

int status_check_interval = 10E3;
int last_status_check_timestamp = 0;
IPAddress ipaddr(WLAN_IPV4_ADDRESS);
IPAddress gateway(WLAN_IPV4_GATEWAY);
IPAddress subnet(WLAN_IPV4_SUBNET_MASK);
IPAddress dns1(WLAN_IPV4_DNS1);
IPAddress dns2(WLAN_IPV4_DNS2);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "north-america.pool.ntp.org", 0, 60E3);

bool initialize_wifi() {
    WiFi.mode(WIFI_STA);

    if (!WiFi.config(ipaddr, gateway, subnet, dns1, dns2)) {
        return false;
    }

    WiFi.begin(WLAN_SSID, WLAN_WPA2KEY);
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount
            // SPIFFS using SPIFFS.end()
            DEBUG_PRINTLN("Start updating " + type);
        })
        .onEnd([]() { DEBUG_PRINTLN("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total) {
            DEBUG_PRINTF1("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            DEBUG_PRINTF1("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR)
                DEBUG_PRINTLN("Auth Failed");
            else if (error == OTA_BEGIN_ERROR)
                DEBUG_PRINTLN("Begin Failed");
            else if (error == OTA_CONNECT_ERROR)
                DEBUG_PRINTLN("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR)
                DEBUG_PRINTLN("Receive Failed");
            else if (error == OTA_END_ERROR)
                DEBUG_PRINTLN("End Failed");
        });

    timeClient.begin();
    DEBUG_PRINTLN("WiFi connecting");
    return true;
}

void check_wifi_status() {
    unsigned long current_millis = millis();
    // if WiFi is down, try reconnecting
    if (current_millis - last_status_check_timestamp >= status_check_interval) {
        if (WiFi.status() != WL_CONNECTED) {
            DEBUG_PRINTF1("WiFi not connected(%d), attempting to connect... ",
                          WiFi.status());
            DEBUG_PRINTLN(WLAN_SSID);
            WiFi.disconnect();
            WiFi.reconnect();
        } else {
            IPAddress ip = WiFi.localIP();
            timeClient.update();
            ArduinoOTA.handle();
            DEBUG_PRINTF2("Connected to network: %s\ncurrent time: %s\t",
                          WLAN_SSID, timeClient.getFormattedTime());
            DEBUG_PRINT("IP: ");
            DEBUG_PRINTLN(ip);
        }

        last_status_check_timestamp = current_millis;
    }
}

uint64_t get_epoch_time() { return timeClient.getEpochTime(); }

IPAddress get_ip() { return WiFi.localIP(); }