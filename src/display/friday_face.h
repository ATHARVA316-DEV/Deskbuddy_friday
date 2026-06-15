#ifndef FRIDAY_FACE_H
#define FRIDAY_FACE_H

#include <Arduino.h>
#include "display_manager.h"
#include "ui_theme.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Face/Character Renderer
//  Refactored from original deskbuddy.ino
// ═══════════════════════════════════════════════════

class FridayFace {
public:
    // ─── Layout constants ──────────────────────────
    static const int HCX = 160;    // Head center X
    static const int HCY = 100;    // Head center Y
    static const int H_OUT = 80;   // Helmet outer radius
    static const int H_IN = 65;    // Helmet inner radius (visor)

    static const int EYE_W = 32;
    static const int EYE_H = 40;
    static const int EYE_R = 13;
    static const int ELX = 130;    // Left eye center X
    static const int ERX = 190;    // Right eye center X
    static const int EYY = 97;     // Eye center Y

    static const int BODY_Y = 180;
    static const int BODY_W = 94;
    static const int BODY_H = 48;

    // ─── Drawing Methods ───────────────────────────

    // Draw the complete static scene (helmet + body)
    static void drawFullScene();

    // Draw helmet (static)
    static void drawHelmet();

    // Draw body (static)
    static void drawBody();

    // Clear only the visor interior
    static void clearVisor();

    // Draw normal eyes with offset and optional blink
    static void drawEyes(int ox, int oy, bool blink);

    // Draw big surprised eyes
    static void drawBigEyes();

    // Draw heart-shaped eyes with pulse animation
    static void drawHeartEyes(int pulse);

    // Draw blush circles
    static void drawBlush();

    // Flicker-free eye update (only redraws if changed)
    static void updateEyes(int ox, int oy, bool blink);

    // ─── Decorative Elements ───────────────────────

    static void drawSparkle(int x, int y, int sz, uint16_t col);
    static void drawHeart(int x, int y, int s);
    static void drawStar(int x, int y, uint16_t col);
    static void eraseHeart(int x, int y, int s);
    static void eraseSparkle(int x, int y, int sz);

    // ─── Animation Sequences ───────────────────────
    // These are self-contained animation loops

    // Idle mode: looking around, blinking (~4 sec)
    static void playIdle(unsigned long durationMs = 4000);

    // Love mode: heart eyes, floating hearts (~6 sec)
    static void playLove(unsigned long durationMs = 6000);

    // Surprised mode: big eyes, head shake (~4 sec)
    static void playSurprised(unsigned long durationMs = 4000);

    // Eye open transition (used during boot)
    static void playEyeOpen();

private:
    // Previous state for flicker-free updates
    static int prevEyeOX, prevEyeOY;
    static bool prevBlink;

    // Heart position tracking for erase
    static int prevH1x, prevH1y, prevH2x, prevH2y;
    static int prevH3x, prevH3y, prevH4x, prevH4y;
    static bool prevHeartsDrawn;

    // Sparkle position tracking
    static int prevSp1x, prevSp1y, prevSp2x, prevSp2y;
    static bool prevSparklesDrawn;

    static void drawGlow(int cx, int cy, int w, int h);
};

#endif // FRIDAY_FACE_H
