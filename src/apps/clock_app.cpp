#include "clock_app.h"
#include <math.h>

// ═══════════════════════════════════════════════════
//  FRIDAY — Clock App Implementation (Touch)
// ═══════════════════════════════════════════════════

// Mode tab touch zones
#define TAB_Y       SCREEN_H - 30
#define TAB_H       28
#define TAB_W       75
#define TAB_START_X 10

// Stopwatch button zones
#define SW_BTN_Y    175
#define SW_BTN_H    30
#define SW_BTN_W    100

// Countdown +/- button zones
#define CD_PLUS_X   200
#define CD_MINUS_X   80
#define CD_BTN_Y     65
#define CD_BTN_W     40
#define CD_BTN_H     35
#define CD_START_X  120
#define CD_START_Y  140
#define CD_START_W   80
#define CD_START_H   30

void ClockApp::onEnter() {
    mode = ClockMode::MODE_DIGITAL;
    lastUpdate = 0;
    swRunning = false;
    swStartTime = 0;
    swElapsed = 0;
    cdMinutes = 5;
    cdRunning = false;
    cdStartTime = 0;
    cdDuration = 0;
    cdFinished = false;
    prevSec = -1;
    prevMin = -1;

    DisplayManager::clearScreen();
    DisplayManager::drawHeader("CLOCK");
    drawModeTabs();
}

void ClockApp::onExit() {
    swRunning = false;
    cdRunning = false;
}

void ClockApp::onInput(InputEvent event) {
    if (event.type == InputType::TOUCH_TAP) {
        int tx = event.touchX;
        int ty = event.touchY;

        // ── Check mode tabs at bottom ──
        if (ty >= TAB_Y && ty <= TAB_Y + TAB_H) {
            for (int i = 0; i < 4; i++) {
                int tabX = TAB_START_X + i * (TAB_W + 5);
                if (InputManager::isInsideRect(tx, ty, tabX, TAB_Y, TAB_W, TAB_H)) {
                    ClockMode newMode = (ClockMode)i;
                    if (newMode != mode) {
                        // Visual feedback: flash the tapped tab
                        const char* tabNames[4] = {"Digital", "Analog", "Stopwatch", "Timer"};
                        DisplayManager::drawTouchButtonActive(tabX, TAB_Y, TAB_W, TAB_H,
                                                               tabNames[i], HUD_CYAN);
                        delay(TOUCH_FEEDBACK_MS);

                        mode = newMode;
                        prevSec = -1;
                        prevMin = -1;
                        DisplayManager::clearScreen();
                        DisplayManager::drawHeader("CLOCK");
                        drawModeTabs();
                    }
                    return;
                }
            }
        }

        // ── Mode-specific touch handling ──
        if (mode == ClockMode::MODE_STOPWATCH) {
            // Tap center area to start/stop
            if (ty >= 50 && ty < SW_BTN_Y) {
                if (swRunning) {
                    swElapsed += millis() - swStartTime;
                    swRunning = false;
                } else {
                    swStartTime = millis();
                    swRunning = true;
                }
                return;
            }
            // Reset button
            if (InputManager::isInsideRect(tx, ty, 110, SW_BTN_Y, SW_BTN_W, SW_BTN_H)) {
                DisplayManager::flashButton(110, SW_BTN_Y, SW_BTN_W, SW_BTN_H, "Reset", HUD_ORANGE);
                swElapsed = 0;
                swRunning = false;
                return;
            }
        }

        if (mode == ClockMode::MODE_COUNTDOWN) {
            if (cdFinished) {
                // Tap to dismiss
                cdFinished = false;
                cdRunning = false;
                DisplayManager::clearScreen();
                DisplayManager::drawHeader("CLOCK");
                drawModeTabs();
                return;
            }

            if (!cdRunning) {
                // + button
                if (InputManager::isInsideRect(tx, ty, CD_PLUS_X, CD_BTN_Y, CD_BTN_W, CD_BTN_H)) {
                    DisplayManager::flashButton(CD_PLUS_X, CD_BTN_Y, CD_BTN_W, CD_BTN_H, "+", HUD_CYAN);
                    cdMinutes = min(60, cdMinutes + 1);
                    return;
                }
                // - button
                if (InputManager::isInsideRect(tx, ty, CD_MINUS_X, CD_BTN_Y, CD_BTN_W, CD_BTN_H)) {
                    DisplayManager::flashButton(CD_MINUS_X, CD_BTN_Y, CD_BTN_W, CD_BTN_H, "-", HUD_CYAN);
                    cdMinutes = max(1, cdMinutes - 1);
                    return;
                }
                // Start button
                if (InputManager::isInsideRect(tx, ty, CD_START_X, CD_START_Y, CD_START_W, CD_START_H)) {
                    DisplayManager::flashButton(CD_START_X, CD_START_Y, CD_START_W, CD_START_H, "Start", HUD_GREEN);
                    cdDuration = (unsigned long)cdMinutes * 60000UL;
                    cdStartTime = millis();
                    cdRunning = true;
                    return;
                }
            } else {
                // Tap to stop
                cdRunning = false;
                return;
            }
        }
    }

    // Swipe left/right to cycle through clock modes
    if (event.type == InputType::TOUCH_SWIPE_LEFT) {
        int newMode = ((int)mode + 1) % 4;
        mode = (ClockMode)newMode;
        prevSec = -1;
        prevMin = -1;
        DisplayManager::clearScreen();
        DisplayManager::drawHeader("CLOCK");
        drawModeTabs();
    }
}

void ClockApp::update() {
    TimeUtils::update();
}

void ClockApp::draw() {
    unsigned long now = millis();
    if (now - lastUpdate < 100) return; // 10 Hz max
    lastUpdate = now;

    switch (mode) {
        case ClockMode::MODE_DIGITAL:   drawDigitalClock(); break;
        case ClockMode::MODE_ANALOG:    drawAnalogClock(); break;
        case ClockMode::MODE_STOPWATCH: drawStopwatch(); break;
        case ClockMode::MODE_COUNTDOWN: drawCountdown(); break;
    }
}

void ClockApp::drawModeTabs() {
    const char* tabNames[4] = {"Digital", "Analog", "Stopwatch", "Timer"};
    for (int i = 0; i < 4; i++) {
        int tabX = TAB_START_X + i * (TAB_W + 5);
        if (i == (int)mode) {
            DisplayManager::drawTouchButtonActive(tabX, TAB_Y, TAB_W, TAB_H,
                                                   tabNames[i], HUD_CYAN);
        } else {
            DisplayManager::drawTouchButton(tabX, TAB_Y, TAB_W, TAB_H,
                                            tabNames[i], HUD_DIM_CYAN);
        }
    }
}

void ClockApp::drawModeIndicator() {
    // Draw mode dots at bottom (kept for backwards compat, but tabs are primary)
    for (int i = 0; i < 4; i++) {
        int x = SCREEN_W / 2 - 30 + i * 20;
        if (i == (int)mode) {
            DisplayManager::tft.fillCircle(x, SCREEN_H - 38, 3, HUD_CYAN);
        } else {
            DisplayManager::tft.drawCircle(x, SCREEN_H - 38, 3, HUD_DIM_CYAN);
        }
    }
}

// ─── Digital Clock ─────────────────────────────────
void ClockApp::drawDigitalClock() {
    auto& tft = DisplayManager::tft;
    int sec = TimeUtils::getSeconds();
    if (sec == prevSec) return;
    prevSec = sec;

    // Clear time area
    tft.fillRect(0, 50, SCREEN_W, 100, HUD_DARK);

    // Large time
    const char* timeStr = TimeUtils::getTimeString();
    DisplayManager::drawCenteredText(timeStr, 65, FONT_HUGE, HUD_BRIGHT_CYAN);

    // Date below
    const char* dateStr = TimeUtils::getDateString();
    DisplayManager::drawCenteredText(dateStr, 130, FONT_SMALL, HUD_LIGHT_GRAY);

    // Day of week
    const char* dayStr = TimeUtils::getDayOfWeek();
    DisplayManager::drawCenteredText(dayStr, 150, FONT_MEDIUM, HUD_CYAN);

    // HUD frame
    DisplayManager::drawHUDFrame(15, 45, 290, 130, HUD_DIM_CYAN);

    // Sync status
    if (TimeUtils::isSynced()) {
        DisplayManager::drawText("NTP", 270, 180, FONT_TINY, HUD_GREEN);
    } else {
        DisplayManager::drawText("NOSYNC", 258, 180, FONT_TINY, HUD_ORANGE);
    }
}

// ─── Analog Clock ──────────────────────────────────
void ClockApp::drawAnalogClock() {
    auto& tft = DisplayManager::tft;
    int sec = TimeUtils::getSeconds();
    if (sec == prevSec) return;
    prevSec = sec;

    int cx = SCREEN_W / 2;
    int cy = 120;
    int r = 75;

    // Clear clock area
    tft.fillRect(cx - r - 5, cy - r - 5, r * 2 + 10, r * 2 + 10, HUD_DARK);

    // Clock face
    tft.drawCircle(cx, cy, r, HUD_CYAN);
    tft.drawCircle(cx, cy, r + 1, HUD_DIM_CYAN);
    tft.drawCircle(cx, cy, r - 1, HUD_DIM_CYAN);

    // Hour markers
    for (int i = 0; i < 12; i++) {
        float angle = i * 30.0f * DEG_TO_RAD - HALF_PI;
        int x1 = cx + (int)((r - 8) * cos(angle));
        int y1 = cy + (int)((r - 8) * sin(angle));
        int x2 = cx + (int)((r - 2) * cos(angle));
        int y2 = cy + (int)((r - 2) * sin(angle));
        tft.drawLine(x1, y1, x2, y2, HUD_WHITE);
    }

    // Minute ticks
    for (int i = 0; i < 60; i++) {
        if (i % 5 == 0) continue;
        float angle = i * 6.0f * DEG_TO_RAD - HALF_PI;
        int x1 = cx + (int)((r - 4) * cos(angle));
        int y1 = cy + (int)((r - 4) * sin(angle));
        tft.drawPixel(x1, y1, HUD_DIM_CYAN);
    }

    // Hands
    int hours = TimeUtils::getHours() % 12;
    int minutes = TimeUtils::getMinutes();
    int seconds = sec;

    // Hour hand
    float hourAngle = (hours + minutes / 60.0f) * 30.0f * DEG_TO_RAD - HALF_PI;
    int hx = cx + (int)(40 * cos(hourAngle));
    int hy = cy + (int)(40 * sin(hourAngle));
    tft.drawLine(cx, cy, hx, hy, HUD_WHITE);
    tft.drawLine(cx + 1, cy, hx + 1, hy, HUD_WHITE);

    // Minute hand
    float minAngle = minutes * 6.0f * DEG_TO_RAD - HALF_PI;
    int mx = cx + (int)(55 * cos(minAngle));
    int my = cy + (int)(55 * sin(minAngle));
    tft.drawLine(cx, cy, mx, my, HUD_CYAN);
    tft.drawLine(cx, cy + 1, mx, my + 1, HUD_CYAN);

    // Second hand
    float secAngle = seconds * 6.0f * DEG_TO_RAD - HALF_PI;
    int sx = cx + (int)(60 * cos(secAngle));
    int sy = cy + (int)(60 * sin(secAngle));
    tft.drawLine(cx, cy, sx, sy, HUD_RED);

    // Center dot
    tft.fillCircle(cx, cy, 3, HUD_BRIGHT_CYAN);
}

// ─── Stopwatch ─────────────────────────────────────
void ClockApp::drawStopwatch() {
    auto& tft = DisplayManager::tft;

    unsigned long elapsed = swElapsed;
    if (swRunning) {
        elapsed += millis() - swStartTime;
    }

    unsigned long ms = elapsed % 1000;
    unsigned long secs = (elapsed / 1000) % 60;
    unsigned long mins = (elapsed / 60000) % 60;
    unsigned long hrs = elapsed / 3600000;

    // Clear display area
    tft.fillRect(0, 50, SCREEN_W, 120, HUD_DARK);

    // Large time
    char buf[20];
    if (hrs > 0) {
        snprintf(buf, sizeof(buf), "%lu:%02lu:%02lu", hrs, mins, secs);
    } else {
        snprintf(buf, sizeof(buf), "%02lu:%02lu", mins, secs);
    }
    DisplayManager::drawCenteredText(buf, 70, FONT_HUGE, HUD_BRIGHT_CYAN);

    // Milliseconds
    char msBuf[8];
    snprintf(msBuf, sizeof(msBuf), ".%03lu", ms);
    DisplayManager::drawCenteredText(msBuf, 120, FONT_MEDIUM, HUD_CYAN);

    // Status
    const char* status = swRunning ? "RUNNING" : (elapsed > 0 ? "PAUSED" : "READY");
    uint16_t statusCol = swRunning ? HUD_GREEN : (elapsed > 0 ? HUD_ORANGE : HUD_GRAY);
    DisplayManager::drawCenteredText(status, 155, FONT_SMALL, statusCol);

    // Touch buttons
    DisplayManager::drawTouchButton(110, SW_BTN_Y, SW_BTN_W, SW_BTN_H, "Reset", HUD_ORANGE);

    // Hint
    DisplayManager::drawCenteredText("Tap center: Start/Stop", SW_BTN_Y + 35, FONT_TINY, HUD_GRAY);

    DisplayManager::drawHUDFrame(20, 45, 280, 130, HUD_DIM_CYAN);
}

// ─── Countdown ─────────────────────────────────────
void ClockApp::drawCountdown() {
    auto& tft = DisplayManager::tft;

    if (cdFinished) {
        // Alarm! Flash screen
        uint16_t col = (millis() / 300) % 2 == 0 ? HUD_RED : HUD_DARK;
        tft.fillRect(0, 50, SCREEN_W, 130, col);
        DisplayManager::drawCenteredText("TIME'S UP!", 100, FONT_LARGE, HUD_WHITE);
        DisplayManager::drawCenteredText("Tap to dismiss", 150, FONT_SMALL, HUD_WHITE);
        return;
    }

    unsigned long remaining = 0;
    float progress = 0;

    if (cdRunning) {
        unsigned long elapsed = millis() - cdStartTime;
        if (elapsed >= cdDuration) {
            cdFinished = true;
            cdRunning = false;
            return;
        }
        remaining = cdDuration - elapsed;
        progress = (float)remaining / (float)cdDuration;
    } else {
        remaining = (unsigned long)cdMinutes * 60000UL;
        progress = 1.0f;
    }

    unsigned long secs = (remaining / 1000) % 60;
    unsigned long mins = remaining / 60000;

    // Clear area
    tft.fillRect(0, 50, SCREEN_W, 130, HUD_DARK);

    // Circular progress arc
    int cx = SCREEN_W / 2;
    int cy = 105;
    int r = 50;

    tft.drawCircle(cx, cy, r, HUD_DIM_CYAN);
    float endAngle = 360.0f * progress;
    DisplayManager::drawArc(cx, cy, r, -90, -90 + endAngle, HUD_CYAN, 3);

    // Time in center
    char buf[10];
    snprintf(buf, sizeof(buf), "%02lu:%02lu", mins, secs);
    DisplayManager::drawCenteredText(buf, cy - 10, FONT_LARGE, HUD_BRIGHT_CYAN);

    if (cdRunning) {
        DisplayManager::drawCenteredText("COUNTING", cy + 25, FONT_TINY, HUD_GREEN);
        DisplayManager::drawCenteredText("Tap to stop", 180, FONT_TINY, HUD_GRAY);
    } else {
        // +/- buttons and start
        DisplayManager::drawTouchButton(CD_MINUS_X, CD_BTN_Y, CD_BTN_W, CD_BTN_H, "-", HUD_CYAN);
        DisplayManager::drawTouchButton(CD_PLUS_X, CD_BTN_Y, CD_BTN_W, CD_BTN_H, "+", HUD_CYAN);

        char setBuf[16];
        snprintf(setBuf, sizeof(setBuf), "%d min", cdMinutes);
        DisplayManager::drawCenteredText(setBuf, cy + 25, FONT_TINY, HUD_GRAY);

        DisplayManager::drawTouchButton(CD_START_X, CD_START_Y, CD_START_W, CD_START_H, "Start", HUD_GREEN);
    }
}
