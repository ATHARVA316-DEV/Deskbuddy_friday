#include "pomodoro_app.h"
#include <math.h>

// ═══════════════════════════════════════════════════
//  FRIDAY — Pomodoro Timer Implementation (Touch)
// ═══════════════════════════════════════════════════

// Setup touch buttons
#define POMO_MINUS_X    60
#define POMO_PLUS_X    210
#define POMO_ADJ_BTN_W  50
#define POMO_ADJ_BTN_H  39
#define POMO_ADJ_BTN_Y  65
#define POMO_START_X   120
#define POMO_START_Y   150
#define POMO_START_W    80
#define POMO_START_H    34

// Finished next button
#define POMO_NEXT_X    120
#define POMO_NEXT_Y    160
#define POMO_NEXT_W     80
#define POMO_NEXT_H     34

void PomodoroApp::onEnter() {
    state = PomodoroState::SETUP;
    workMinutes = 25;
    breakMinutes = 5;
    sessionStart = 0;
    sessionDuration = 0;
    pauseElapsed = 0;
    completedSessions = 0;
    totalSessions = 8;
    lastDraw = 0;
    forceRedraw = true;

    DisplayManager::clearScreen();
    DisplayManager::drawHeader("POMODORO");
}

void PomodoroApp::onExit() {
    // Nothing
}

void PomodoroApp::onInput(InputEvent event) {
    if (event.type != InputType::TOUCH_TAP) return;

    int tx = event.touchX;
    int ty = event.touchY;

    switch (state) {
        case PomodoroState::SETUP:
            // - button
            if (InputManager::isInsideRect(tx, ty, POMO_MINUS_X, POMO_ADJ_BTN_Y,
                    POMO_ADJ_BTN_W, POMO_ADJ_BTN_H)) {
                DisplayManager::flashButton(POMO_MINUS_X, POMO_ADJ_BTN_Y,
                        POMO_ADJ_BTN_W, POMO_ADJ_BTN_H, "-5", HUD_CYAN);
                workMinutes = max(1, workMinutes - 5);
                breakMinutes = max(1, workMinutes / 5);
                forceRedraw = true;
            }
            // + button
            if (InputManager::isInsideRect(tx, ty, POMO_PLUS_X, POMO_ADJ_BTN_Y,
                    POMO_ADJ_BTN_W, POMO_ADJ_BTN_H)) {
                DisplayManager::flashButton(POMO_PLUS_X, POMO_ADJ_BTN_Y,
                        POMO_ADJ_BTN_W, POMO_ADJ_BTN_H, "+5", HUD_CYAN);
                workMinutes = min(60, workMinutes + 5);
                breakMinutes = max(1, workMinutes / 5);
                forceRedraw = true;
            }
            // Start button
            if (InputManager::isInsideRect(tx, ty, POMO_START_X, POMO_START_Y,
                    POMO_START_W, POMO_START_H)) {
                DisplayManager::flashButton(POMO_START_X, POMO_START_Y,
                        POMO_START_W, POMO_START_H, "Start", HUD_GREEN);
                state = PomodoroState::WORK;
                sessionDuration = (unsigned long)workMinutes * 60000UL;
                sessionStart = millis();
                pauseElapsed = 0;
                forceRedraw = true;
                DisplayManager::clearScreen();
                DisplayManager::drawHeader("POMODORO");
            }
            break;

        case PomodoroState::WORK:
        case PomodoroState::BREAK_TIME:
            // Tap center area to pause
            if (ty >= 50 && ty <= 170) {
                pauseElapsed += millis() - sessionStart;
                state = PomodoroState::PAUSED;
                forceRedraw = true;
            }
            break;

        case PomodoroState::PAUSED:
            // Tap center to resume
            if (ty >= 50 && ty <= 170) {
                sessionStart = millis();
                if (pauseElapsed < (unsigned long)workMinutes * 60000UL) {
                    state = PomodoroState::WORK;
                } else {
                    state = PomodoroState::BREAK_TIME;
                }
                sessionDuration -= pauseElapsed;
                pauseElapsed = 0;
                forceRedraw = true;
            }
            break;

        case PomodoroState::FINISHED:
            // Next button
            if (InputManager::isInsideRect(tx, ty, POMO_NEXT_X, POMO_NEXT_Y,
                    POMO_NEXT_W, POMO_NEXT_H)) {
                DisplayManager::flashButton(POMO_NEXT_X, POMO_NEXT_Y,
                        POMO_NEXT_W, POMO_NEXT_H, "Next", HUD_GREEN);
                if (completedSessions < totalSessions) {
                    state = PomodoroState::BREAK_TIME;
                    sessionDuration = (unsigned long)breakMinutes * 60000UL;
                    sessionStart = millis();
                    pauseElapsed = 0;
                    forceRedraw = true;
                    DisplayManager::clearScreen();
                    DisplayManager::drawHeader("POMODORO");
                } else {
                    state = PomodoroState::SETUP;
                    completedSessions = 0;
                    forceRedraw = true;
                    DisplayManager::clearScreen();
                    DisplayManager::drawHeader("POMODORO");
                }
            }
            break;
    }
}

void PomodoroApp::update() {
    if (state == PomodoroState::WORK || state == PomodoroState::BREAK_TIME) {
        unsigned long elapsed = millis() - sessionStart;
        if (elapsed >= sessionDuration) {
            if (state == PomodoroState::WORK) {
                completedSessions++;
                state = PomodoroState::FINISHED;
            } else {
                // Break finished, start next work session
                state = PomodoroState::WORK;
                sessionDuration = (unsigned long)workMinutes * 60000UL;
                sessionStart = millis();
                pauseElapsed = 0;
            }
            forceRedraw = true;
            DisplayManager::clearScreen();
            DisplayManager::drawHeader("POMODORO");
        }
    }
}

void PomodoroApp::draw() {
    unsigned long now = millis();
    if (!forceRedraw && (now - lastDraw < 200)) return;
    lastDraw = now;
    forceRedraw = false;

    switch (state) {
        case PomodoroState::SETUP:
            drawSetup();
            break;
        case PomodoroState::WORK:
        case PomodoroState::BREAK_TIME:
        case PomodoroState::PAUSED:
            drawTimer();
            break;
        case PomodoroState::FINISHED:
            drawFinished();
            break;
    }
}

void PomodoroApp::drawSetup() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("POMODORO");

    // Work time display
    DisplayManager::drawCenteredText("WORK TIME", 45, FONT_SMALL, HUD_LIGHT_GRAY);

    char buf[8];
    snprintf(buf, sizeof(buf), "%d", workMinutes);
    DisplayManager::drawCenteredText(buf, 70, FONT_HUGE, HUD_BRIGHT_CYAN);
    DisplayManager::drawCenteredText("minutes", 115, FONT_SMALL, HUD_DIM_CYAN);

    // +/- buttons
    DisplayManager::drawTouchButton(POMO_MINUS_X, POMO_ADJ_BTN_Y,
                                    POMO_ADJ_BTN_W, POMO_ADJ_BTN_H, "-5", HUD_CYAN);
    DisplayManager::drawTouchButton(POMO_PLUS_X, POMO_ADJ_BTN_Y,
                                    POMO_ADJ_BTN_W, POMO_ADJ_BTN_H, "+5", HUD_CYAN);

    // Break time
    char brkBuf[20];
    snprintf(brkBuf, sizeof(brkBuf), "Break: %d min", breakMinutes);
    DisplayManager::drawCenteredText(brkBuf, 135, FONT_SMALL, HUD_GRAY);

    // Start button
    DisplayManager::drawTouchButton(POMO_START_X, POMO_START_Y,
                                    POMO_START_W, POMO_START_H, "Start", HUD_GREEN);

    DisplayManager::drawHUDFrame(10, 35, 300, 135, HUD_DIM_CYAN);
}

void PomodoroApp::drawTimer() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("POMODORO");

    // Calculate remaining time
    unsigned long elapsed;
    if (state == PomodoroState::PAUSED) {
        elapsed = pauseElapsed;
    } else {
        elapsed = millis() - sessionStart;
    }

    unsigned long remaining = (elapsed < sessionDuration) ? (sessionDuration - elapsed) : 0;
    float progress = 1.0f - ((float)remaining / (float)sessionDuration);

    unsigned long remSec = (remaining / 1000) % 60;
    unsigned long remMin = remaining / 60000;

    // Mode label
    const char* modeLabel;
    uint16_t modeColor;
    if (state == PomodoroState::WORK) {
        modeLabel = "WORK";
        modeColor = HUD_RED;
    } else if (state == PomodoroState::BREAK_TIME) {
        modeLabel = "BREAK";
        modeColor = HUD_GREEN;
    } else {
        modeLabel = "PAUSED";
        modeColor = HUD_ORANGE;
    }

    // Circular progress
    drawCircularProgress(progress, modeColor);

    // Time in center of circle
    int cy = 115;
    char timeBuf[10];
    snprintf(timeBuf, sizeof(timeBuf), "%02lu:%02lu", remMin, remSec);
    DisplayManager::drawCenteredText(timeBuf, cy - 12, FONT_LARGE, HUD_BRIGHT_CYAN);

    // Mode label below time
    DisplayManager::drawCenteredText(modeLabel, cy + 16, FONT_SMALL, modeColor);

    // Session counter at bottom
    drawSessionDots();

    // Controls hint
    if (state == PomodoroState::PAUSED) {
        DisplayManager::drawCenteredText("Tap: Resume", SCREEN_H - 14, FONT_TINY, HUD_GRAY);
    } else {
        DisplayManager::drawCenteredText("Tap: Pause", SCREEN_H - 14, FONT_TINY, HUD_GRAY);
    }
}

void PomodoroApp::drawFinished() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("POMODORO");

    // Celebration
    DisplayManager::drawCenteredText("SESSION", 60, FONT_MEDIUM, HUD_GREEN);
    DisplayManager::drawCenteredText("COMPLETE!", 85, FONT_LARGE, HUD_BRIGHT_CYAN);

    char buf[16];
    snprintf(buf, sizeof(buf), "%d / %d", completedSessions, totalSessions);
    DisplayManager::drawCenteredText(buf, 130, FONT_MEDIUM, HUD_WHITE);

    drawSessionDots();

    // Next button
    DisplayManager::drawTouchButton(POMO_NEXT_X, POMO_NEXT_Y,
                                    POMO_NEXT_W, POMO_NEXT_H, "Next", HUD_GREEN);

    DisplayManager::drawHUDFrame(30, 50, 260, 100, HUD_CYAN);
}

void PomodoroApp::drawSessionDots() {
    int dotY = 195;
    int totalW = totalSessions * 16;
    int startX = (SCREEN_W - totalW) / 2;

    DisplayManager::drawCenteredText("Sessions:", 180, FONT_TINY, HUD_GRAY);

    for (int i = 0; i < totalSessions; i++) {
        int x = startX + i * 16 + 8;
        if (i < completedSessions) {
            DisplayManager::tft.fillCircle(x, dotY, 5, HUD_CYAN);
        } else {
            DisplayManager::tft.drawCircle(x, dotY, 5, HUD_DIM_CYAN);
        }
    }
}

void PomodoroApp::drawCircularProgress(float progress, uint16_t color) {
    int cx = SCREEN_W / 2;
    int cy = 115;
    int r = 55;

    // Background ring
    DisplayManager::tft.drawCircle(cx, cy, r, HUD_DIM_CYAN);
    DisplayManager::tft.drawCircle(cx, cy, r - 1, HUD_DIM_CYAN);

    // Progress arc
    float endAngle = -90 + 360.0f * progress;
    DisplayManager::drawArc(cx, cy, r, -90, endAngle, color, 4);
}
