#ifndef CONF_H
#define CONF_H

#define LOADCELL_INIT_TARE_VALUE -6293.5
// calibration factor: read raw loadcell value with known weight attached
// divide reading(g) by known weight mass(g)
#define LOADCELL_CALIBRATION_FACTOR -109

#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 2

#define EADC_ALERT_PIN 0
//controls when the eADC comparator sends an alert
//high must be >= low
#define EADC_COMPARATOR_THRESHOLD_HIGH 0x0250
#define EADC_COMPARATOR_THRESHOLD_LOW 0x0100

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
#ifndef WEBSERVER_PORT
#define WEBSERVER_PORT 80
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