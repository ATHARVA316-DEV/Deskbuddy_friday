#ifndef RADIAL_MENU_H
#define RADIAL_MENU_H

#include <Arduino.h>
#include "../friday_core/input_manager.h"
#include "../friday_core/state_machine.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Radial Menu (Arc Reactor Style)
//  Touch-based circular launcher
// ═══════════════════════════════════════════════════

// Forward declaration
enum class AppState;

class RadialMenu {
public:
    static void init();
    static void update();
    static void draw();
    static void onInput(InputEvent event);

    static AppState getSelectedAppState();
    static int getSelectedIndex();

private:
    static int selectedIndex;
    static float currentAngle;       // Current rotation offset (radians)
    static float targetAngle;        // Target angle for animation
    static bool needsRedraw;
    static unsigned long lastDrawTime;
    static int prevSelectedIndex;

    // Pre-computed sin/cos lookup (360 entries)
    static float sinTable[360];
    static float cosTable[360];

    // Icon positions (computed each frame)
    static int iconX[7];
    static int iconY[7];

    static void computeLayout();
    static void drawCore();
    static void drawOrbitRing();
    static void drawIcons();
    static void drawSelectedLabel();
    static void drawHUDDecorations();
    static void initTrigTables();

    // Touch hit-test: returns icon index at (x,y) or -1
    static int hitTestIcon(int x, int y);

    static float lerpAngle(float current, float target, float t);
    static int angleLookupIndex(float radians);

    // Menu item definitions
    static const char* menuNames[7];
    static const uint8_t* menuIcons[7];
};

#endif // RADIAL_MENU_H
