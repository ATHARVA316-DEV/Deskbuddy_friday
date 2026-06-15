#include "radial_menu.h"
#include "menu_icons.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"
#include "../config.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Radial Menu Implementation (Touch)
//  Arc Reactor / Iron Man HUD circular launcher
// ═══════════════════════════════════════════════════

// Static member initialization
int RadialMenu::selectedIndex = 0;
float RadialMenu::currentAngle = 0;
float RadialMenu::targetAngle = 0;
bool RadialMenu::needsRedraw = true;
unsigned long RadialMenu::lastDrawTime = 0;
int RadialMenu::prevSelectedIndex = -1;

float RadialMenu::sinTable[360];
float RadialMenu::cosTable[360];
int RadialMenu::iconX[7];
int RadialMenu::iconY[7];

// Menu definitions
const char* RadialMenu::menuNames[7] = {
    "Clock",
    "Weather",
    "Calculator",
    "System",
    "Toolbox",
    "Pomodoro",
    "DeskBuddy"
};

const uint8_t* RadialMenu::menuIcons[7] = {
    icon_clock,
    icon_weather,
    icon_calc,
    icon_sysmon,
    icon_toolbox,
    icon_pomodoro,
    icon_deskbuddy
};

void RadialMenu::initTrigTables() {
    for (int i = 0; i < 360; i++) {
        float rad = i * DEG_TO_RAD;
        sinTable[i] = sin(rad);
        cosTable[i] = cos(rad);
    }
}

void RadialMenu::init() {
    static bool tablesInitialized = false;
    if (!tablesInitialized) {
        initTrigTables();
        tablesInitialized = true;
    }
    selectedIndex = 0;
    currentAngle = 0;
    targetAngle = 0;
    needsRedraw = true;
    prevSelectedIndex = -1;

    // Full initial draw
    DisplayManager::clearScreen();
    computeLayout();
    drawCore();
    drawOrbitRing();
    drawIcons();
    drawSelectedLabel();
    drawHUDDecorations();
    lastDrawTime = millis();
}

void RadialMenu::onInput(InputEvent event) {
    if (event.type == InputType::TOUCH_TAP) {
        // Hit-test: did user tap an icon?
        int hitIdx = hitTestIcon(event.touchX, event.touchY);
        if (hitIdx >= 0) {
            selectedIndex = hitIdx;
            targetAngle = -selectedIndex * (TWO_PI / MENU_ITEMS);
            needsRedraw = true;

            // Visual feedback: briefly highlight the tapped icon before launching
            draw(); // Redraw with new selection highlighted
            delay(TOUCH_FEEDBACK_MS);

            // Launch the tapped app directly
            AppState appState = getSelectedAppState();
            StateMachine::transitionTo(appState);
            return;
        }

        // Tap on center core → launch current selection
        int dx = event.touchX - MENU_CENTER_X;
        int dy = event.touchY - MENU_CENTER_Y;
        if ((dx * dx + dy * dy) < 30 * 30) {
            // Visual feedback on center core
            auto& tft = DisplayManager::tft;
            tft.fillCircle(MENU_CENTER_X, MENU_CENTER_Y, 14, HUD_BRIGHT_CYAN);
            delay(TOUCH_FEEDBACK_MS);

            AppState appState = getSelectedAppState();
            StateMachine::transitionTo(appState);
            return;
        }

        // Tap on sides → rotate menu
        if (event.touchX < 100) {
            // Rotate selection backward
            selectedIndex = (selectedIndex - 1 + MENU_ITEMS) % MENU_ITEMS;
            targetAngle = -selectedIndex * (TWO_PI / MENU_ITEMS);
            needsRedraw = true;
            return;
        } else if (event.touchX > 220 && event.touchY < 60) {
            // Check if mini Iron Man was tapped (top right corner)
            int imX = SCREEN_W - 25;
            int imY = 25;
            if (abs(event.touchX - imX) < 30 && abs(event.touchY - imY) < 30) {
                // Interactive Iron Man easter egg! DANCE!
                for (int d = 0; d < 6; d++) {
                    DisplayManager::drawIronManFigure(imX, imY, 1, d % 2 == 0 ? IronManState::STANDING : IronManState::DANCING); // Toggle frame
                    delay(150);
                }
                // Back to normal
                DisplayManager::drawIronManFigure(imX, imY, 1, IronManState::STANDING);
                needsRedraw = true;
                return;
            }
        } else if (event.touchX > 220) {
            // Rotate selection forward
            selectedIndex = (selectedIndex + 1) % MENU_ITEMS;
            targetAngle = -selectedIndex * (TWO_PI / MENU_ITEMS);
            needsRedraw = true;
            return;
        }
    }

    if (event.type == InputType::TOUCH_SWIPE_LEFT) {
        // Rotate selection forward
        selectedIndex = (selectedIndex + 1) % MENU_ITEMS;
        targetAngle = -selectedIndex * (TWO_PI / MENU_ITEMS);
        needsRedraw = true;
    }

    if (event.type == InputType::TOUCH_SWIPE_RIGHT) {
        // Rotate selection backward (but don't go back from menu)
        selectedIndex = (selectedIndex - 1 + MENU_ITEMS) % MENU_ITEMS;
        targetAngle = -selectedIndex * (TWO_PI / MENU_ITEMS);
        needsRedraw = true;
    }

    // Drag gesture: rotate menu proportional to horizontal drag
    if (event.type == InputType::TOUCH_DRAG) {
        float dragStep = (float)event.deltaX * 0.008f; // Scale drag to radians
        targetAngle += dragStep;
        // Snap to nearest item after drag
        float itemStep = TWO_PI / MENU_ITEMS;
        int nearest = -(int)round(targetAngle / itemStep);
        nearest = ((nearest % MENU_ITEMS) + MENU_ITEMS) % MENU_ITEMS;
        selectedIndex = nearest;
        targetAngle = -selectedIndex * itemStep;
        needsRedraw = true;
    }
}

void RadialMenu::update() {
    // Smooth angle interpolation (fast lerp for responsive feel)
    if (abs(currentAngle - targetAngle) > 0.01f) {
        currentAngle = lerpAngle(currentAngle, targetAngle, 0.30f);
        needsRedraw = true;
    }
}

void RadialMenu::draw() {
    if (!needsRedraw && prevSelectedIndex == selectedIndex) return;

    unsigned long now = millis();
    if (now - lastDrawTime < 25) return; // Cap at ~40 FPS for smoother animation
    lastDrawTime = now;

    // Fast hardware block erase that preserves corner HUD elements but clears everything else
    // This issues 1 high-speed SPI command instead of 200+, preventing WDT crashes
    DisplayManager::tft.fillRect(30, 20, 260, 200, MENU_BG);

    computeLayout();
    drawHUDDecorations();
    drawOrbitRing();
    drawIcons();
    drawCore();
    drawSelectedLabel();

    prevSelectedIndex = selectedIndex;
    needsRedraw = (abs(currentAngle - targetAngle) > 0.01f);
}

AppState RadialMenu::getSelectedAppState() {
    switch (selectedIndex) {
        case 0: return AppState::APP_CLOCK;
        case 1: return AppState::APP_WEATHER;
        case 2: return AppState::APP_CALCULATOR;
        case 3: return AppState::APP_SYSMON;
        case 4: return AppState::APP_TOOLBOX;
        case 5: return AppState::APP_POMODORO;
        case 6: return AppState::APP_DESKBUDDY;
        default: return AppState::APP_CLOCK;
    }
}

int RadialMenu::getSelectedIndex() {
    return selectedIndex;
}

// ─── Touch Hit-Test ────────────────────────────────

int RadialMenu::hitTestIcon(int x, int y) {
    for (int i = 0; i < MENU_ITEMS; i++) {
        int dx = x - iconX[i];
        int dy = y - iconY[i];
        // Generous hit radius: icon size + 10px padding for finger-friendly targeting
        int hitRadius = (i == selectedIndex) ? MENU_ICON_R_SEL + 10 : MENU_ICON_R + 10;
        if ((dx * dx + dy * dy) < (hitRadius * hitRadius)) {
            return i;
        }
    }
    return -1;
}

// ─── Layout Computation ────────────────────────────

void RadialMenu::computeLayout() {
    float angleStep = TWO_PI / MENU_ITEMS;
    float startAngle = -HALF_PI + currentAngle; // Start from top (12 o'clock)

    for (int i = 0; i < MENU_ITEMS; i++) {
        float angle = startAngle + i * angleStep;
        int lookupIdx = angleLookupIndex(angle);

        int radius = MENU_RADIUS;

        // Pull selected icon toward center
        if (i == selectedIndex) {
            radius -= MENU_PULL_IN;
        }

        iconX[i] = MENU_CENTER_X + (int)(radius * cosTable[lookupIdx]);
        iconY[i] = MENU_CENTER_Y + (int)(radius * sinTable[lookupIdx]);
    }
}

// ─── Drawing Functions ─────────────────────────────

void RadialMenu::drawCore() {
    auto& tft = DisplayManager::tft;
    int cx = MENU_CENTER_X;
    int cy = MENU_CENTER_Y;

    // Arc reactor core — concentric rings
    tft.fillCircle(cx, cy, 22, HUD_DARK_BLUE);
    tft.drawCircle(cx, cy, 22, MENU_CORE_COL);
    tft.drawCircle(cx, cy, 18, MENU_CORE_RING);
    tft.fillCircle(cx, cy, 14, HUD_DARK);
    tft.drawCircle(cx, cy, 14, MENU_CORE_COL);

    // Inner glow dot
    tft.fillCircle(cx, cy, 6, HUD_CYAN);
    tft.fillCircle(cx, cy, 3, HUD_BRIGHT_CYAN);

    // "FRIDAY" text inside core
    tft.setTextSize(1);
    tft.setTextColor(HUD_DIM_CYAN);
    // Text is too small for the core, so we skip it and use the glow dot
}

void RadialMenu::drawOrbitRing() {
    auto& tft = DisplayManager::tft;
    int cx = MENU_CENTER_X;
    int cy = MENU_CENTER_Y;

    // Main orbit ring (thin, dim)
    tft.drawCircle(cx, cy, MENU_RADIUS, MENU_RING_COL);
    tft.drawCircle(cx, cy, MENU_RADIUS + 1, MENU_RING_COL);

    // Outer decorative ring
    tft.drawCircle(cx, cy, MENU_RADIUS + 10, HUD_DARK_BLUE);
}

void RadialMenu::drawIcons() {
    auto& tft = DisplayManager::tft;

    for (int i = 0; i < MENU_ITEMS; i++) {
        int x = iconX[i];
        int y = iconY[i];
        bool isSelected = (i == selectedIndex);

        if (isSelected) {
            // ── Selected: enlarged with glow ──
            // Glow halo
            tft.fillCircle(x, y, MENU_ICON_R_SEL + 4, MENU_GLOW_COL);
            // Icon background
            tft.fillCircle(x, y, MENU_ICON_R_SEL, HUD_DARK_BLUE);
            // Bright border
            tft.drawCircle(x, y, MENU_ICON_R_SEL, MENU_ICON_SELECTED);
            tft.drawCircle(x, y, MENU_ICON_R_SEL - 1, MENU_ICON_SELECTED);

            // Draw icon bitmap (centered in circle)
            tft.drawBitmap(x - 8, y - 8, menuIcons[i], 16, 16, MENU_ICON_SELECTED);
        } else {
            // ── Unselected: smaller, dim ──
            tft.fillCircle(x, y, MENU_ICON_R, HUD_DARK_BLUE);
            tft.drawCircle(x, y, MENU_ICON_R, MENU_ICON_NORMAL);

            // Draw icon bitmap
            tft.drawBitmap(x - 8, y - 8, menuIcons[i], 16, 16, MENU_ICON_NORMAL);
        }
    }
}

void RadialMenu::drawSelectedLabel() {
    // App name below the center core
    DisplayManager::drawCenteredText(menuNames[selectedIndex], MENU_CENTER_Y + 30,
                                    FONT_SMALL, MENU_LABEL_COL);

    // Small arrow indicator
    auto& tft = DisplayManager::tft;
    int cx = MENU_CENTER_X;
    int y = MENU_CENTER_Y + 42;
    tft.drawTriangle(cx - 4, y, cx + 4, y, cx, y + 4, HUD_DIM_CYAN);
}

void RadialMenu::drawHUDDecorations() {
    auto& tft = DisplayManager::tft;

    // Corner accents (Iron Man HUD style)
    // Top-left
    tft.drawFastHLine(2, 2, 20, HUD_DIM_CYAN);
    tft.drawFastVLine(2, 2, 20, HUD_DIM_CYAN);
    // Top-right
    tft.drawFastHLine(SCREEN_W - 22, 2, 20, HUD_DIM_CYAN);
    tft.drawFastVLine(SCREEN_W - 3, 2, 20, HUD_DIM_CYAN);
    // Bottom-left
    tft.drawFastHLine(2, SCREEN_H - 3, 20, HUD_DIM_CYAN);
    tft.drawFastVLine(2, SCREEN_H - 22, 20, HUD_DIM_CYAN);
    // Bottom-right
    tft.drawFastHLine(SCREEN_W - 22, SCREEN_H - 3, 20, HUD_DIM_CYAN);
    tft.drawFastVLine(SCREEN_W - 3, SCREEN_H - 22, 20, HUD_DIM_CYAN);

    // Top/bottom info text
    DisplayManager::drawCenteredText("FRIDAY v1.0", 4, FONT_TINY, HUD_GRAY);

    // Bottom status — touch hints
    DisplayManager::drawCenteredText("Tap: Open  Swipe: Navigate", SCREEN_H - 12,
                                    FONT_TINY, HUD_GRAY);

    // Rotating tick marks around outer ring
    int cx = MENU_CENTER_X;
    int cy = MENU_CENTER_Y;
    int tickR = MENU_RADIUS + 14;

    for (int a = 0; a < 360; a += 15) {
        int idx = a % 360;
        int x1 = cx + (int)(tickR * cosTable[idx]);
        int y1 = cy + (int)(tickR * sinTable[idx]);
        int x2 = cx + (int)((tickR + 3) * cosTable[idx]);
        int y2 = cy + (int)((tickR + 3) * sinTable[idx]);
        tft.drawLine(x1, y1, x2, y2, MENU_TICK_COL);
    }

    // Mini Interactive Iron Man (Top Right)
    DisplayManager::drawIronManFigure(SCREEN_W - 25, 25, 1, IronManState::STANDING);
}

// ─── Utility ───────────────────────────────────────

float RadialMenu::lerpAngle(float current, float target, float t) {
    return current + (target - current) * t;
}

int RadialMenu::angleLookupIndex(float radians) {
    int deg = (int)(radians * RAD_TO_DEG) % 360;
    if (deg < 0) deg += 360;
    return deg;
}
