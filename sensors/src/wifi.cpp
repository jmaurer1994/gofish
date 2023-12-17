#include "wifi.h"

WiFiModuleState current_state = WIFI_UNINITIALIZED;

int status_check_interval = 10E3;
int last_status_check_timestamp = 0;
IPAddress ipaddr(WLAN_IPV4_ADDRESS);
IPAddress gateway(WLAN_IPV4_GATEWAY);
IPAddress subnet(WLAN_IPV4_SUBNET_MASK);

bool initialize_wifi() {
  WiFi.mode(WIFI_STA);

  if (!WiFi.config(ipaddr, gateway, subnet)) {
    current_state = WIFI_FAILED;
    return false;
  }

  WiFi.begin(WLAN_SSID, WLAN_WPA2KEY);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  return true;
}

void check_wifi_status() {
  unsigned long current_millis = millis();
  // if WiFi is down, try reconnecting
  if (current_millis - last_status_check_timestamp >= status_check_interval) {
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      WiFi.reconnect();
    } else {
        Serial.println("Wifi Connected");
    }

    last_status_check_timestamp = current_millis;
  }
}

IPAddress get_ip() { return WiFi.localIP(); }