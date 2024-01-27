#include "network.h"

IPAddress ipaddr(WLAN_IPV4_ADDRESS);
IPAddress gateway(WLAN_IPV4_GATEWAY);
IPAddress subnet(WLAN_IPV4_SUBNET_MASK);
IPAddress dns1(WLAN_IPV4_DNS1);
IPAddress dns2(WLAN_IPV4_DNS2);

WiFiUDP ntpUDP;
// no offset because it offsets the epoch time value (per library doc)
// https://github.com/arduino-libraries/NTPClient
NTPClient timeClient(ntpUDP, NTP_SYNC_URL, 0, NTP_SYNC_INTERVAL_MILLIS);
bool initialize_network() {
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
    DEBUG_PRINTLN("Network initialized");
    return true;
}

void run_network_checks() {
    static int64_t last_status_check_timestamp = -WLAN_CHECK_INTERVAL_MILLIS; // should run at start
    uint64_t current_millis = millis();

    if (current_millis - last_status_check_timestamp <
        WLAN_CHECK_INTERVAL_MILLIS) {
        return; // not time to to check
    }

    last_status_check_timestamp = current_millis;
    // check wifi
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTF1("WiFi not connected(%d), attempting to connect to network: ",
                      WiFi.status());
        DEBUG_PRINTLN(WLAN_SSID);
        WiFi.disconnect();
        WiFi.reconnect();
        if (WiFi.status() != WL_CONNECTED) {
            return;
        }
    }

    // sync with NTP server if needed
    timeClient.update();

    // handle any OTA events
    ArduinoOTA.handle();

    DEBUG_PRINTF2("Connected to network: %s\n%s\t", WLAN_SSID,
                  timeClient.getFormattedTime());

    IPAddress ip = WiFi.localIP();
    DEBUG_PRINT("IP: ");
    DEBUG_PRINTLN(ip);
    return;
}

uint64_t get_epoch_time(){ return timeClient.getEpochTime(); }

IPAddress get_ip() { return WiFi.localIP(); }