#ifndef CLOCK_APP_H
#define CLOCK_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"
#include "../utils/time_utils.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Clock App (Touch)
//  Digital, Analog, Stopwatch, Countdown
// ═══════════════════════════════════════════════════

enum class ClockMode {
    MODE_DIGITAL,
    MODE_ANALOG,
    MODE_STOPWATCH,
    MODE_COUNTDOWN
};

class ClockApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "Clock"; }
    bool needsRedraw() override { return true; }

private:
    ClockMode mode;
    unsigned long lastUpdate;

    // Stopwatch state
    bool swRunning;
    unsigned long swStartTime;
    unsigned long swElapsed;

    // Countdown state
    int cdMinutes;
    bool cdRunning;
    unsigned long cdStartTime;
    unsigned long cdDuration;
    bool cdFinished;

    // Previous values for partial redraw
    int prevSec;
    int prevMin;

    void drawDigitalClock();
    void drawAnalogClock();
    void drawStopwatch();
    void drawCountdown();
    void drawModeIndicator();
    void drawModeTabs();
};

#endif // CLOCK_APP_H
