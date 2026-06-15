#include "sysmon_app.h"
#include <WiFi.h>
#include <esp_system.h>
#include "../utils/wifi_manager.h"
#include "../friday_core/state_machine.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — System Monitor Implementation (Touch)
// ═══════════════════════════════════════════════════

// Button positions
#define SETTINGS_REFRESH_X  125
#define SETTINGS_BTN_Y      210
#define SETTINGS_BTN_W      70
#define SETTINGS_BTN_H      30

void SysMonApp::onEnter() {
    lastDraw = 0;
    DisplayManager::clearScreen();
    DisplayManager::drawHeader("SETTINGS");
}

void SysMonApp::onExit() {
    // Nothing
}

void SysMonApp::onInput(InputEvent event) {
    // Tap refresh button
    if (event.type == InputType::TOUCH_TAP) {
        if (InputManager::isInsideRect(event.touchX, event.touchY,
                SETTINGS_REFRESH_X, SETTINGS_BTN_Y,
                SETTINGS_BTN_W, SETTINGS_BTN_H)) {
            DisplayManager::flashButton(SETTINGS_REFRESH_X, SETTINGS_BTN_Y,
                    SETTINGS_BTN_W, SETTINGS_BTN_H, "Refresh", HUD_CYAN);
            lastDraw = 0; // Force redraw
        } else {
            // Tap anywhere else forces a redraw
            lastDraw = 0;
        }
    }
}

void SysMonApp::update() {
    // Updates handled in draw
}

void SysMonApp::draw() {
    unsigned long now = millis();
    // Fast update rate for real-time touch coordinate debugging
    if (now - lastDraw < 50) return; 
    lastDraw = now;

    drawDashboard();
}

void SysMonApp::drawDashboard() {
    auto& tft = DisplayManager::tft;

    // Clear content area
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("SETTINGS");

    int y = 36;
    int labelX = 12;
    int barX = 100;
    int barW = 160;

    // ─── Free RAM ──────────────────────────────────
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t totalHeap = 320000; // Approximate total SRAM
    float ramPct = (float)freeHeap / (float)totalHeap;

    char ramBuf[16];
    snprintf(ramBuf, sizeof(ramBuf), "%luKB", (unsigned long)(freeHeap / 1024));
    drawBarWithLabel("RAM", ramBuf, ramPct, labelX, y, barW, HUD_GREEN);

    // ─── CPU Frequency ─────────────────────────────
    y += 32;
    uint32_t cpuFreq = ESP.getCpuFreqMHz();
    float cpuPct = (float)cpuFreq / 240.0f;

    char cpuBuf[16];
    snprintf(cpuBuf, sizeof(cpuBuf), "%luMHz", (unsigned long)cpuFreq);
    drawBarWithLabel("CPU", cpuBuf, cpuPct, labelX, y, barW, HUD_CYAN);

    // ─── WiFi Signal ───────────────────────────────
    y += 32;
    bool wifiConn = WiFiManager::isConnected();
    int rssi = wifiConn ? WiFiManager::getRSSI() : -100;
    float wifiPct = wifiConn ? constrain(map(rssi, -100, -30, 0, 100) / 100.0f, 0.0f, 1.0f) : 0;

    char wifiBuf[16];
    if (wifiConn) {
        snprintf(wifiBuf, sizeof(wifiBuf), "%ddBm", rssi);
    } else {
        strcpy(wifiBuf, "N/A");
    }
    uint16_t wifiCol = wifiConn ? HUD_CYAN : HUD_RED;
    drawBarWithLabel("WiFi", wifiBuf, wifiPct, labelX, y, barW, wifiCol);

    // WiFi status text
    tft.setTextSize(FONT_TINY);
    tft.setTextColor(wifiConn ? HUD_GREEN : HUD_ORANGE);
    tft.setCursor(barX, y + 14);
    tft.print(wifiConn ? "Connected" : "Disconnected");

    // ─── Uptime ────────────────────────────────────
    y += 38;
    const char* uptime = formatUptime(millis());
    DisplayManager::drawText("Uptime", labelX, y, FONT_SMALL, HUD_LIGHT_GRAY);
    DisplayManager::drawText(uptime, barX, y, FONT_SMALL, HUD_WHITE);

    // ─── Chip Info ─────────────────────────────────
    y += 24;
    tft.drawFastHLine(labelX, y, SCREEN_W - 2 * labelX, APP_BORDER);
    y += 8;

    DisplayManager::drawText("Chip", labelX, y, FONT_TINY, HUD_GRAY);
    DisplayManager::drawText("ESP32-WROOM-32", barX, y, FONT_TINY, HUD_LIGHT_GRAY);

    y += 14;
    DisplayManager::drawText("Flash", labelX, y, FONT_TINY, HUD_GRAY);
    char flashBuf[16];
    snprintf(flashBuf, sizeof(flashBuf), "%luMB", (unsigned long)(ESP.getFlashChipSize() / (1024 * 1024)));
    DisplayManager::drawText(flashBuf, barX, y, FONT_TINY, HUD_LIGHT_GRAY);

    y += 14;
    DisplayManager::drawText("SDK", labelX, y, FONT_TINY, HUD_GRAY);
    DisplayManager::drawText(ESP.getSdkVersion(), barX, y, FONT_TINY, HUD_LIGHT_GRAY);

    // WiFi IP
    if (wifiConn) {
        y += 14;
        DisplayManager::drawText("IP", labelX, y, FONT_TINY, HUD_GRAY);
        const char* ip = WiFiManager::getLocalIP();
        DisplayManager::drawText(ip, barX, y, FONT_TINY, HUD_LIGHT_GRAY);
    }
    // Touch Calibration Debug Overlay
    y += 18;
    tft.drawFastHLine(labelX, y, SCREEN_W - 2 * labelX, APP_BORDER);
    y += 8;
    
    int rawX, rawY;
    InputManager::getRawPoint(rawX, rawY);
    int scrX = InputManager::getLastX();
    int scrY = InputManager::getLastY();
    
    char touchBuf[64];
    snprintf(touchBuf, sizeof(touchBuf), "Raw: %d,%d | Scr: %d,%d", rawX, rawY, scrX, scrY);
    DisplayManager::drawText("Touch", labelX, y, FONT_TINY, HUD_ORANGE);
    DisplayManager::drawText(touchBuf, barX - 30, y, FONT_TINY, HUD_BRIGHT_CYAN);

    // Buttons
    DisplayManager::drawTouchButton(SETTINGS_REFRESH_X, SETTINGS_BTN_Y,
                                    SETTINGS_BTN_W, SETTINGS_BTN_H,
                                    "Refresh", HUD_DIM_CYAN);

    // HUD frame
    DisplayManager::drawHUDFrame(5, HEADER_HEIGHT + 2, 310, SCREEN_H - HEADER_HEIGHT - 4, HUD_DIM_CYAN);
}

void SysMonApp::drawBarWithLabel(const char* label, const char* value, float pct,
                                  int x, int y, int w, uint16_t color) {
    // Label
    DisplayManager::drawText(label, x, y, FONT_SMALL, HUD_LIGHT_GRAY);

    // Progress bar
    int barX = x + 50;
    DisplayManager::drawProgressBar(barX, y + 2, w, 10, pct, color, APP_BAR_BG);

    // Value text
    DisplayManager::drawText(value, barX + w + 6, y, FONT_SMALL, HUD_WHITE);
}

const char* SysMonApp::formatUptime(unsigned long ms) {
    unsigned long totalSec = ms / 1000;
    unsigned long hours = totalSec / 3600;
    unsigned long minutes = (totalSec % 3600) / 60;
    unsigned long seconds = totalSec % 60;

    static char buf[16];
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", hours, minutes, seconds);
    return buf;
}
