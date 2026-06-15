#include "time_utils.h"
#include "wifi_manager.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Time Utilities Implementation
// ═══════════════════════════════════════════════════

WiFiUDP TimeUtils::ntpUDP;
NTPClient TimeUtils::timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_SEC, NTP_SYNC_INTERVAL);
bool TimeUtils::synced = false;
unsigned long TimeUtils::lastSyncTime = 0;

const char* TimeUtils::dayNames[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

const char* TimeUtils::monthNames[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void TimeUtils::init() {
    if (WiFiManager::isConnected()) {
        timeClient.begin();
        synced = timeClient.update();
        if (synced) {
            lastSyncTime = millis();
            Serial.print("NTP synced. Time: ");
            Serial.println(timeClient.getFormattedTime());
        }
    }
}

void TimeUtils::update() {
    if (WiFiManager::isConnected()) {
        // Periodic re-sync
        if (millis() - lastSyncTime > NTP_SYNC_INTERVAL) {
            if (timeClient.update()) {
                synced = true;
                lastSyncTime = millis();
            }
        }
    }
}

int TimeUtils::getHours() {
    return timeClient.getHours();
}

int TimeUtils::getMinutes() {
    return timeClient.getMinutes();
}

int TimeUtils::getSeconds() {
    return timeClient.getSeconds();
}

unsigned long TimeUtils::getEpochTime() {
    return timeClient.getEpochTime();
}

const char* TimeUtils::getTimeString() {
    static char buf[12];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", getHours(), getMinutes(), getSeconds());
    return buf;
}

const char* TimeUtils::getTimeShort() {
    static char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", getHours(), getMinutes());
    return buf;
}

const char* TimeUtils::getDateString() {
    unsigned long epoch = getEpochTime();
    // Simple date calculation from epoch
    // Days since epoch (Jan 1, 1970)
    unsigned long days = epoch / 86400;
    int dayOfWeek = (days + 4) % 7; // Jan 1, 1970 was Thursday (4)

    // Calculate year, month, day
    int year = 1970;
    while (true) {
        int daysInYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 366 : 365;
        if (days < (unsigned long)daysInYear) break;
        days -= daysInYear;
        year++;
    }

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) daysInMonth[1] = 29;

    int month = 0;
    while (month < 12 && days >= (unsigned long)daysInMonth[month]) {
        days -= daysInMonth[month];
        month++;
    }
    int day = days + 1;

    static char buf[32];
    snprintf(buf, sizeof(buf), "%s, %d %s %d",
             dayNames[dayOfWeek], day, monthNames[month], year);
    return buf;
}

const char* TimeUtils::getDayOfWeek() {
    int day = timeClient.getDay();
    return dayNames[day];
}

const char* TimeUtils::getGreeting() {
    int hour = getHours();
    if (hour < 12) return "Good Morning";
    if (hour < 17) return "Good Afternoon";
    return "Good Evening";
}

bool TimeUtils::isSynced() {
    return synced;
}

void TimeUtils::forceSync() {
    if (WiFiManager::isConnected()) {
        synced = timeClient.forceUpdate();
        if (synced) lastSyncTime = millis();
    }
}
