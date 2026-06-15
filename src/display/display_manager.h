#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include "../config.h"
#include "ui_theme.h"

enum class IronManState {
    STANDING,
    DANCING,
    FLYING,
    LOVE,
    SAD,
    HEARTBROKEN,
    HAPPY,
    SHOOTING
};

// ═══════════════════════════════════════════════════
//  FRIDAY — Display Manager
//  Wraps TFT with common drawing primitives
// ═══════════════════════════════════════════════════

class DisplayManager {
public:
    static Adafruit_ILI9341 tft;

    static void init();

    // ─── Text Helpers ──────────────────────────────
    static void drawCenteredText(const char* text, int y, uint8_t size, uint16_t color);
    static void drawRightText(const char* text, int x, int y, uint8_t size, uint16_t color);
    static void drawText(const char* text, int x, int y, uint8_t size, uint16_t color);

    // ─── Shape Helpers ─────────────────────────────
    static void drawArc(int cx, int cy, int r, float startAngle, float endAngle,
                        uint16_t color, int thickness = 1);
    static void drawGlowCircle(int cx, int cy, int r, uint16_t coreColor,
                               uint16_t glowColor, int glowSize = 3);
    static void drawProgressBar(int x, int y, int w, int h, float progress,
                                uint16_t fillColor, uint16_t bgColor);
    static void drawHUDFrame(int x, int y, int w, int h, uint16_t color);
    static void drawDashedHLine(int x, int y, int w, uint16_t color, int dashLen = 4);
    static void drawDashedVLine(int x, int y, int h, uint16_t color, int dashLen = 4);

    // ─── Touch Button Helpers ──────────────────────
    // Draw a rounded-rect touch button with centered label
    static void drawTouchButton(int x, int y, int w, int h,
                                const char* label, uint16_t color,
                                uint16_t textColor = HUD_WHITE);
    // Draw a touch button with pressed/highlighted state
    static void drawTouchButtonActive(int x, int y, int w, int h,
                                      const char* label, uint16_t color);

    // ─── Back Button ───────────────────────────────
    // Standard back button drawn in every app header
    static void drawBackButton(bool pressed = false);

    // ─── Touch Feedback ────────────────────────────
    // Brief touch ripple at a point (expanding circle)
    static void drawTouchRipple(int x, int y, uint16_t color = HUD_TOUCH_HI);

    // Flash a button in its pressed state, then restore
    static void flashButton(int x, int y, int w, int h,
                            const char* label, uint16_t color);

    // ─── Screen Helpers ────────────────────────────
    static void clearScreen();
    static void drawHeader(const char* title);
    static void fillArea(int x, int y, int w, int h, uint16_t color);

    // ─── Transition Effects ────────────────────────────
    static void drawIronManFigure(int x, int y, int scale = 1, IronManState state = IronManState::STANDING);
    static void ironManFlightTransition(uint16_t bgColor = HUD_DARK);
    static void irisOpen(uint16_t color = HUD_DARK);
    static void irisClose(uint16_t color = HUD_DARK);
    static void scanWipe(uint16_t color = HUD_DARK);
};

#endif // DISPLAY_MANAGER_H
