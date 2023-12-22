#ifndef CONF_H
#define CONF_H

#define DEBUG

#define INITIAL_SCALE_TARE_VALUE -6293.5 // TODO:recalc

#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 2

#define EADC_ALERT_PIN 0

// should be provided by user_build_params.ini
#ifndef WLAN_IPV4_ADDRESS
#define WLAN_IPV4_ADDRESS 192, 168, 25, 100
#endif
#ifndef WLAN_IPV4_GATEWAY
#define WLAN_IPV4_GATEWAY 192, 168, 25, 1
#endif
#ifndef WLAN_IPV4_SUBNET_MASK
#define WLAN_IPV4_SUBNET_MASK 255, 255, 255, 0
#endif
#ifndef WLAN_SSID
#define WLAN_SSID "SSID"
#endif
#ifndef WLAN_WPA2KEY
#define WLAN_WPA2KEY "password123"
#endif
#ifndef INITIAL_STARTUP_DELAY
#define INITIAL_STARTUP_DELAY 0
#endif
#ifndef SERIAL_BAUD_RATE
#define SERIAL_BAUD_RATE 115200
#endif

#ifdef DEBUG
#define DEBUG_BEGIN(x) Serial.begin(x)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF1(txt, x) Serial.printf(txt, x)
#define DEBUG_PRINTF2(txt, x, y) Serial.printf(txt, x, y)
#define DEBUG_PRINTF3(txt, x, y, z) Serial.printf(txt, x, y, z)
#else
#define DEBUG_BEGIN(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x) 
#define DEBUG_PRINTF1(txt, x)
#define DEBUG_PRINTF2(txt, x, y)
#define DEBUG_PRINTF3(txt, x, y, z)
#endif
#endif