#include "state_machine.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"
#include "../menu/radial_menu.h"
#include "../apps/clock_app.h"
#include "../apps/weather_app.h"
#include "../apps/calculator_app.h"
#include "../apps/sysmon_app.h"
#include "../apps/toolbox_app.h"
#include "../apps/pomodoro_app.h"
#include "../apps/touch_debug_app.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — State Machine Implementation (Touch)
// ═══════════════════════════════════════════════════

// Static member initialization
AppState StateMachine::currentState = AppState::BOOT;
AppState StateMachine::previousState = AppState::BOOT;
AppBase* StateMachine::activeApp = nullptr;
bool StateMachine::needsRedraw = true;
unsigned long StateMachine::lastActivityTime = 0;

// App instances
static ClockApp      clockApp;
static WeatherApp    weatherApp;
static CalculatorApp calculatorApp;
static SysMonApp     sysmonApp;
static ToolboxApp    toolboxApp;
static PomodoroApp   pomodoroApp;
static TouchDebugApp touchDebugApp;

AppBase* StateMachine::apps[7] = {nullptr};

// ─── Power menu touch button positions ─────────────
#define PWRMENU_CONFIRM_X   30
#define PWRMENU_CONFIRM_Y  115
#define PWRMENU_CONFIRM_W  120
#define PWRMENU_CONFIRM_H   45
#define PWRMENU_CANCEL_X   170
#define PWRMENU_CANCEL_Y   115
#define PWRMENU_CANCEL_W   120
#define PWRMENU_CANCEL_H    45

void StateMachine::init() {
    apps[0] = &clockApp;
    apps[1] = &weatherApp;
    apps[2] = &calculatorApp;
    apps[3] = &sysmonApp;
    apps[4] = &toolboxApp;
    apps[5] = &pomodoroApp;
    apps[6] = &touchDebugApp;

    currentState = AppState::BOOT;
    previousState = AppState::BOOT;
    activeApp = nullptr;
    needsRedraw = true;
    lastActivityTime = millis();

    // Transition explicitly to MENU so RadialMenu::init() gets called
    transitionTo(AppState::MENU);
}

void StateMachine::handleInput(InputEvent event) {
    if (event.type == InputType::NONE) return;

    // Ignore release and drag at the state machine level (apps can handle them)
    if (event.type == InputType::TOUCH_RELEASE) return;
    if (event.type == InputType::TOUCH_DRAG && currentState != AppState::MENU) {
        // Forward drag to active app only
        if (activeApp) activeApp->onInput(event);
        return;
    }

    lastActivityTime = millis();

    switch (currentState) {
        case AppState::MENU:
            // Forward all touch events to radial menu (long-press no longer opens power menu)
            RadialMenu::onInput(event);
            break;

        case AppState::SLEEP:
            // Any tap wakes up
            if (event.type == InputType::TOUCH_TAP) {
                exitSleep();
            }
            break;

        case AppState::POWER_MENU:
            if (event.type == InputType::TOUCH_TAP) {
                // Check which button was tapped
                // Confirm action removed: power off disabled.
                // Touching confirm previously triggered deep sleep; that behavior is intentionally disabled.
                if (InputManager::isInsideRect(event.touchX, event.touchY,
                        PWRMENU_CANCEL_X, PWRMENU_CANCEL_Y,
                        PWRMENU_CANCEL_W, PWRMENU_CANCEL_H)) {
                    // Visual feedback then cancel
                    DisplayManager::flashButton(PWRMENU_CANCEL_X, PWRMENU_CANCEL_Y,
                            PWRMENU_CANCEL_W, PWRMENU_CANCEL_H, "Cancel", HUD_CYAN);
                    transitionTo(AppState::MENU);
                }
            } else if (event.type == InputType::TOUCH_SWIPE_RIGHT) {
                // Swipe right cancels power menu
                transitionTo(AppState::MENU);
            }
            break;

        default:
            // In an app — check global swipe-right OR back button tap for back
            if (event.type == InputType::TOUCH_SWIPE_RIGHT || 
               (event.type == InputType::TOUCH_TAP && 
                InputManager::isInsideRect(event.touchX, event.touchY, BACK_BTN_X, BACK_BTN_Y, BACK_BTN_W, BACK_BTN_H))) {
                
                // Visual feedback if tapped
                if (event.type == InputType::TOUCH_TAP) {
                    DisplayManager::drawBackButton(true);
                    delay(TOUCH_FEEDBACK_MS);
                }

                // Check if the current app consumes the back action
                if (activeApp && activeApp->onBack()) {
                    // Visual feedback might need reset
                    if (event.type == InputType::TOUCH_TAP) {
                        DisplayManager::drawBackButton(false);
                    }
                    return; // App consumed it
                }

                // App did not consume it, go back to main menu
                transitionTo(AppState::MENU);
            } else if (activeApp) {
                activeApp->onInput(event);
            }
            break;
    }
}

void StateMachine::update() {
    switch (currentState) {
        case AppState::MENU:
            RadialMenu::update();
            break;

        case AppState::SLEEP:
            // Nothing to update in sleep
            break;

        case AppState::POWER_MENU:
            // Static screen
            break;

        default:
            if (activeApp) {
                activeApp->update();
            }
            break;
    }
}



void StateMachine::draw() {
    switch (currentState) {
        case AppState::MENU:
            RadialMenu::draw();
            break;

        case AppState::SLEEP:
            // Screen is off
            break;

        case AppState::POWER_MENU:
            if (needsRedraw) {
                showPowerMenu();
                needsRedraw = false;
            }
            break;

        default:
            if (activeApp) {
                activeApp->draw();
            }
            break;
    }
}

void StateMachine::transitionTo(AppState newState) {
    // Exit current app
    if (activeApp) {
        activeApp->onExit();
        activeApp = nullptr;
    }

    previousState = currentState;
    currentState = newState;
    needsRedraw = true;

    // Screen transition
    if (newState != AppState::SLEEP) {
        // Use Iron Man Flying transition for all standard state changes
        DisplayManager::ironManFlightTransition(APP_BG);
    }

    // Enter new state
    switch (newState) {
        case AppState::MENU:
            RadialMenu::init();
            break;

        default:
            activeApp = getAppForState(newState);
            if (activeApp) {
                activeApp->onEnter();
            }
            break;
    }
}

AppState StateMachine::getCurrentState() {
    return currentState;
}

AppBase* StateMachine::getActiveApp() {
    return activeApp;
}

AppBase* StateMachine::getAppForState(AppState state) {
    int idx = getAppIndex(state);
    if (idx >= 0 && idx < 7) return apps[idx];
    return nullptr;
}

int StateMachine::getAppIndex(AppState state) {
    switch (state) {
        case AppState::APP_CLOCK:      return 0;
        case AppState::APP_WEATHER:    return 1;
        case AppState::APP_CALCULATOR: return 2;
        case AppState::APP_SYSMON:     return 3;
        case AppState::APP_TOOLBOX:    return 4;
        case AppState::APP_POMODORO:   return 5;
        case AppState::APP_DESKBUDDY:  return 6;
        default: return -1;
    }
}

void StateMachine::enterSleep() {
    previousState = currentState;
    currentState = AppState::SLEEP;

    // Fade out effect
    DisplayManager::scanWipe(HUD_BLACK);

    // Turn off display backlight (if controlled via GPIO)
    // For now just show black screen
    DisplayManager::tft.fillScreen(HUD_BLACK);
}

void StateMachine::exitSleep() {
    currentState = AppState::MENU;
    needsRedraw = true;

    // Wake animation
    DisplayManager::clearScreen();
    DisplayManager::drawCenteredText("FRIDAY", 90, FONT_XLARGE, HUD_CYAN);
    DisplayManager::drawCenteredText("Systems Online", 130, FONT_SMALL, HUD_DIM_CYAN);
    delay(800);

    RadialMenu::init();
}

void StateMachine::showPowerMenu() {
    currentState = AppState::POWER_MENU;
    needsRedraw = false;

    DisplayManager::clearScreen();
    DisplayManager::drawHUDFrame(30, 40, 260, 140, HUD_CYAN);

    DisplayManager::drawCenteredText("Power options disabled", 60, FONT_MEDIUM, HUD_RED);

    DisplayManager::tft.drawFastHLine(50, 85, 220, APP_BORDER);

    DisplayManager::drawCenteredText("Press cancel to return", 95, FONT_TINY, HUD_GRAY);

    // Draw only Cancel touch button (confirm removed)
    DisplayManager::drawTouchButton(PWRMENU_CANCEL_X, PWRMENU_CANCEL_Y,
                                    PWRMENU_CANCEL_W, PWRMENU_CANCEL_H,
                                    "Cancel", HUD_CYAN);

    // Decorative arc
    DisplayManager::drawArc(160, 120, 60, 0, 360, HUD_DIM_CYAN, 1);
}
