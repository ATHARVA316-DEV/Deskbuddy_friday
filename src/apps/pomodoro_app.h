#ifndef POMODORO_APP_H
#define POMODORO_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Pomodoro Timer App
//  25/5, Custom, Session Counter
// ═══════════════════════════════════════════════════

enum class PomodoroState {
    SETUP,      // Configure time
    WORK,       // Work session
    BREAK_TIME, // Break session
    PAUSED,
    FINISHED    // Session complete notification
};

class PomodoroApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "Pomodoro"; }
    bool needsRedraw() override { return true; }

private:
    PomodoroState state;
    int workMinutes;
    int breakMinutes;
    unsigned long sessionStart;
    unsigned long sessionDuration;
    unsigned long pauseElapsed;
    int completedSessions;
    int totalSessions;       // Target sessions (default 8)
    unsigned long lastDraw;
    bool forceRedraw;

    void drawSetup();
    void drawTimer();
    void drawFinished();
    void drawSessionDots();
    void drawCircularProgress(float progress, uint16_t color);
};

#endif // POMODORO_APP_H
