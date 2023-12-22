#include "wifi.h"

int status_check_interval = 10E3;
int last_status_check_timestamp = 0;
IPAddress ipaddr(WLAN_IPV4_ADDRESS);
IPAddress gateway(WLAN_IPV4_GATEWAY);
IPAddress subnet(WLAN_IPV4_SUBNET_MASK);

bool initialize_wifi() {
  WiFi.mode(WIFI_STA);

  if (!WiFi.config(ipaddr, gateway, subnet)) {
    return false;
  }

  WiFi.begin(WLAN_SSID, WLAN_WPA2KEY);
  DEBUG_PRINTLN("WiFi connecting");
  return true;
}

void check_wifi_status() {
  unsigned long current_millis = millis();
  // if WiFi is down, try reconnecting
  if (current_millis - last_status_check_timestamp >= status_check_interval) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTF1("WiFi not connected, connecting to: %s\n", WLAN_SSID);
      WiFi.disconnect();
      WiFi.reconnect();
    } else {
        DEBUG_PRINTF1("Connected to %s\n", WLAN_SSID);
    }

    last_status_check_timestamp = current_millis;
  }
}

IPAddress get_ip() { return WiFi.localIP(); }