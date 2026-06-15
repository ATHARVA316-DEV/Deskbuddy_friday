#ifndef APP_BASE_H
#define APP_BASE_H

#include "input_manager.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — App Base Class
//  Abstract interface all apps must implement
// ═══════════════════════════════════════════════════

class AppBase {
public:
    virtual ~AppBase() {}

    // Called once when the app becomes active
    virtual void onEnter() = 0;

    // Called once when leaving the app
    virtual void onExit() = 0;

    // Called every loop iteration while app is active
    virtual void update() = 0;

    // Handle user input events
    virtual void onInput(InputEvent event) = 0;

    // Render the current app state to display
    virtual void draw() = 0;

    // Return the display name of this app
    virtual const char* getName() = 0;

    // Whether this app needs frequent redraws (e.g., animations)
    virtual bool needsRedraw() { return false; }

    // Returns true if the app consumed the back action (e.g. returning from nested menu)
    // Returns false if the app wants to let StateMachine return to the Main Menu
    virtual bool onBack() { return false; }
};

#endif // APP_BASE_H
