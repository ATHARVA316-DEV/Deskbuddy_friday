#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "../config.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Time Utilities
//  NTP sync + time formatting + greeting logic
// ═══════════════════════════════════════════════════

class TimeUtils {
public:
    static void init();
    static void update();

    // Get current time components
    static int getHours();
    static int getMinutes();
    static int getSeconds();
    static unsigned long getEpochTime();

    // Formatted strings
    static const char* getTimeString();      // "HH:MM:SS"
    static const char* getTimeShort();       // "HH:MM"
    static const char* getDateString();      // "Mon, 13 Jun 2026"
    static const char* getDayOfWeek();       // "Monday"

    // Greeting based on time of day
    static const char* getGreeting();   // "Good Morning" / "Afternoon" / "Evening"

    // Sync status
    static bool isSynced();
    static void forceSync();

private:
    static WiFiUDP ntpUDP;
    static NTPClient timeClient;
    static bool synced;
    static unsigned long lastSyncTime;

    static const char* dayNames[7];
    static const char* monthNames[12];
};

#endif // TIME_UTILS_H
