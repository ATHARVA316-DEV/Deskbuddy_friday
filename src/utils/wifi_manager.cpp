#include "wifi_manager.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — WiFi Manager Implementation
// ═══════════════════════════════════════════════════

bool WiFiManager::connected = false;
unsigned long WiFiManager::lastReconnectAttempt = 0;

void WiFiManager::connect() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_TIMEOUT) {
        delay(250);
    }

    connected = (WiFi.status() == WL_CONNECTED);

    if (connected) {
        Serial.print("WiFi connected. IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi connection failed — running offline.");
    }
}

bool WiFiManager::isConnected() {
    connected = (WiFi.status() == WL_CONNECTED);
    return connected;
}

void WiFiManager::reconnectIfNeeded() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 30000) { // Try every 30 seconds
            lastReconnectAttempt = now;
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            Serial.println("WiFi reconnecting...");
        }
        connected = false;
    } else {
        connected = true;
    }
}

int WiFiManager::getRSSI() {
    if (!isConnected()) return -100;
    return WiFi.RSSI();
}

const char* WiFiManager::getSSID() {
    return WIFI_SSID;
}

const char* WiFiManager::getLocalIP() {
    if (!isConnected()) return "N/A";
    static char ipBuf[16];
    strncpy(ipBuf, WiFi.localIP().toString().c_str(), sizeof(ipBuf));
    ipBuf[sizeof(ipBuf) - 1] = '\0';
    return ipBuf;
}
