#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "app_base.h"
#include "input_manager.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — State Machine
//  Global application state management
// ═══════════════════════════════════════════════════

enum class AppState {
    BOOT,
    MENU,
    SLEEP,
    POWER_MENU,
    APP_CLOCK,
    APP_WEATHER,
    APP_CALCULATOR,
    APP_SYSMON,
    APP_TOOLBOX,
    APP_POMODORO,
    APP_DESKBUDDY
};

class StateMachine {
public:
    static void init();
    static void handleInput(InputEvent event);
    static void update();
    static void draw();
    static void transitionTo(AppState newState);

    static AppState getCurrentState();
    static AppBase* getActiveApp();

private:
    static AppState currentState;
    static AppState previousState;
    static AppBase* activeApp;
    static bool needsRedraw;
    static unsigned long lastActivityTime;

    // App instances (allocated once at init)
    static AppBase* apps[7];

    static AppBase* getAppForState(AppState state);
    static int getAppIndex(AppState state);
    static void enterSleep();
    static void exitSleep();
    static void showPowerMenu();
};

#endif // STATE_MACHINE_H
