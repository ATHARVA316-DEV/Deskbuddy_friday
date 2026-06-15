#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "../config.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — WiFi Manager
// ═══════════════════════════════════════════════════

class WiFiManager {
public:
    static void connect();
    static bool isConnected();
    static void reconnectIfNeeded();
    static int getRSSI();
    static const char* getSSID();
    static const char* getLocalIP();

private:
    static bool connected;
    static unsigned long lastReconnectAttempt;
};

#endif // WIFI_MANAGER_H
