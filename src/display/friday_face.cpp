#include "friday_face.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Face Renderer Implementation
//  Refactored from original deskbuddy.ino
// ═══════════════════════════════════════════════════

// Static member initialization
int FridayFace::prevEyeOX = 0;
int FridayFace::prevEyeOY = 0;
bool FridayFace::prevBlink = false;

int FridayFace::prevH1x = 0, FridayFace::prevH1y = 0;
int FridayFace::prevH2x = 0, FridayFace::prevH2y = 0;
int FridayFace::prevH3x = 0, FridayFace::prevH3y = 0;
int FridayFace::prevH4x = 0, FridayFace::prevH4y = 0;
bool FridayFace::prevHeartsDrawn = false;

int FridayFace::prevSp1x = 0, FridayFace::prevSp1y = 0;
int FridayFace::prevSp2x = 0, FridayFace::prevSp2y = 0;
bool FridayFace::prevSparklesDrawn = false;

// ─── Core Drawing ──────────────────────────────────

void FridayFace::drawFullScene() {
    DisplayManager::tft.fillScreen(HUD_DARK);
    drawBody();
    drawHelmet();
}

void FridayFace::clearVisor() {
    DisplayManager::tft.fillCircle(HCX, HCY, H_IN, CHAR_VISOR);
}

void FridayFace::drawHelmet() {
    auto& tft = DisplayManager::tft;
    tft.fillCircle(HCX, HCY, H_OUT, CHAR_HELMET);
    tft.fillCircle(HCX, HCY, H_IN, CHAR_VISOR);

    // Highlight arc on helmet
    for (int a = 125; a < 210; a++) {
        float r = a * 0.01745329f;
        for (int d = 3; d <= 5; d++) {
            int x = HCX + (int)((H_OUT - d) * cos(r));
            int y = HCY + (int)((H_OUT - d) * sin(r));
            tft.drawPixel(x, y, CHAR_HELMET_HI);
        }
    }

    // Ear pieces
    tft.fillCircle(HCX - H_OUT + 4, HCY + 6, 11, CHAR_HELMET);
    tft.fillCircle(HCX + H_OUT - 4, HCY + 6, 11, CHAR_HELMET);
    tft.fillCircle(HCX - H_OUT + 4, HCY + 6, 5, CHAR_BODY_SHADOW);
    tft.fillCircle(HCX + H_OUT - 4, HCY + 6, 5, CHAR_BODY_SHADOW);
}

void FridayFace::drawBody() {
    auto& tft = DisplayManager::tft;
    int bx = HCX - BODY_W / 2;
    tft.fillRoundRect(bx, BODY_Y, BODY_W, BODY_H, 20, CHAR_BODY);
    tft.fillRoundRect(bx + 4, BODY_Y + 22, BODY_W - 8, BODY_H - 20, 15, CHAR_BODY_SHADOW);

    // Feet
    tft.fillCircle(HCX - 24, BODY_Y + BODY_H - 2, 11, CHAR_BODY);
    tft.fillCircle(HCX + 24, BODY_Y + BODY_H - 2, 11, CHAR_BODY);

    // Label
    tft.setTextColor(CHAR_EYE_GLOW);
    tft.setTextSize(1);
    tft.setCursor(HCX - 17, BODY_Y + 7);
    tft.print("FRIDAY");
}

void FridayFace::drawBlush() {
    auto& tft = DisplayManager::tft;
    tft.fillCircle(ELX - 18, EYY + 18, 7, HUD_PINK);
    tft.fillCircle(ERX + 18, EYY + 18, 7, HUD_PINK);
}

void FridayFace::drawGlow(int cx, int cy, int w, int h) {
    DisplayManager::tft.fillRoundRect(cx - w / 2 - 4, cy - h / 2 - 4,
                                      w + 8, h + 8, EYE_R + 3, CHAR_EYE_DIM);
}

// ─── Eye Modes ─────────────────────────────────────

void FridayFace::drawEyes(int ox, int oy, bool blink) {
    auto& tft = DisplayManager::tft;
    int lx = ELX + ox;
    int rx = ERX + ox;
    int ey = EYY + oy;

    if (blink) {
        for (int i = -13; i <= 13; i++) {
            int c = -(i * i) / 12;
            tft.drawPixel(lx + i, ey + c,     CHAR_EYE_GLOW);
            tft.drawPixel(lx + i, ey + c + 1, CHAR_EYE_GLOW);
            tft.drawPixel(lx + i, ey + c + 2, CHAR_EYE_GLOW);
            tft.drawPixel(rx + i, ey + c,     CHAR_EYE_GLOW);
            tft.drawPixel(rx + i, ey + c + 1, CHAR_EYE_GLOW);
            tft.drawPixel(rx + i, ey + c + 2, CHAR_EYE_GLOW);
        }
        return;
    }

    drawGlow(lx, ey, EYE_W, EYE_H);
    drawGlow(rx, ey, EYE_W, EYE_H);

    tft.fillRoundRect(lx - EYE_W / 2, ey - EYE_H / 2, EYE_W, EYE_H, EYE_R, CHAR_EYE_GLOW);
    tft.fillRoundRect(rx - EYE_W / 2, ey - EYE_H / 2, EYE_W, EYE_H, EYE_R, CHAR_EYE_GLOW);

    // Specular highlights
    tft.fillCircle(lx - 7, ey - 10, 5, CHAR_EYE_HI);
    tft.fillCircle(rx - 7, ey - 10, 5, CHAR_EYE_HI);
    tft.fillCircle(lx + 6, ey + 7, 2, CHAR_EYE_HI);
    tft.fillCircle(rx + 6, ey + 7, 2, CHAR_EYE_HI);
    tft.fillCircle(lx - 2, ey - 14, 2, CHAR_EYE_HI);
    tft.fillCircle(rx - 2, ey - 14, 2, CHAR_EYE_HI);
}

void FridayFace::drawBigEyes() {
    auto& tft = DisplayManager::tft;
    int lx = ELX, rx = ERX;
    int ey = EYY - 3;
    int w = EYE_W + 10, h = EYE_H + 14, r = 16;

    drawGlow(lx, ey, w, h);
    drawGlow(rx, ey, w, h);

    tft.fillRoundRect(lx - w / 2, ey - h / 2, w, h, r, CHAR_EYE_GLOW);
    tft.fillRoundRect(rx - w / 2, ey - h / 2, w, h, r, CHAR_EYE_GLOW);

    tft.fillCircle(lx - 9, ey - 14, 6, CHAR_EYE_HI);
    tft.fillCircle(rx - 9, ey - 14, 6, CHAR_EYE_HI);
    tft.fillCircle(lx + 7, ey + 8, 3, CHAR_EYE_HI);
    tft.fillCircle(rx + 7, ey + 8, 3, CHAR_EYE_HI);
    tft.fillCircle(lx - 3, ey - 20, 2, CHAR_EYE_HI);
    tft.fillCircle(rx - 3, ey - 20, 2, CHAR_EYE_HI);
}

void FridayFace::drawHeartEyes(int pulse) {
    auto& tft = DisplayManager::tft;
    int s = 8 + pulse;

    for (int side = 0; side < 2; side++) {
        int cx = (side == 0) ? ELX : ERX;
        int cy = EYY;

        tft.fillCircle(cx, cy + 3, s + 6, CHAR_EYE_DIM);
        tft.fillCircle(cx - s + 2, cy - 2, s, CHAR_HEART);
        tft.fillCircle(cx + s - 2, cy - 2, s, CHAR_HEART);
        tft.fillTriangle(
            cx - s * 2 + 2, cy + 1,
            cx + s * 2 - 2, cy + 1,
            cx, cy + s * 2 + 4,
            CHAR_HEART
        );
        tft.fillCircle(cx - s + 3, cy - s + 2, 3, CHAR_HEART_HI);
    }
}

void FridayFace::updateEyes(int ox, int oy, bool blink) {
    if (ox == prevEyeOX && oy == prevEyeOY && blink == prevBlink) return;
    clearVisor();
    drawEyes(ox, oy, blink);
    drawBlush();
    prevEyeOX = ox;
    prevEyeOY = oy;
    prevBlink = blink;
}

// ─── Decorative Elements ───────────────────────────

void FridayFace::drawSparkle(int x, int y, int sz, uint16_t col) {
    auto& tft = DisplayManager::tft;
    tft.drawFastVLine(x, y - sz, sz * 2 + 1, col);
    tft.drawFastHLine(x - sz, y, sz * 2 + 1, col);
    for (int i = 1; i < sz; i++) {
        tft.drawPixel(x - i, y - i, col);
        tft.drawPixel(x + i, y - i, col);
        tft.drawPixel(x - i, y + i, col);
        tft.drawPixel(x + i, y + i, col);
    }
}

void FridayFace::drawHeart(int x, int y, int s) {
    auto& tft = DisplayManager::tft;
    tft.fillCircle(x - s, y, s, CHAR_HEART);
    tft.fillCircle(x + s, y, s, CHAR_HEART);
    tft.fillTriangle(x - s * 2, y + s / 2,
                     x + s * 2, y + s / 2,
                     x, y + s * 3, CHAR_HEART);
    tft.fillCircle(x - s + 1, y - s / 2, max(1, s / 3), CHAR_HEART_HI);
}

void FridayFace::drawStar(int x, int y, uint16_t col) {
    auto& tft = DisplayManager::tft;
    tft.drawPixel(x, y - 4, col);
    tft.drawPixel(x, y - 3, col);
    tft.drawFastHLine(x - 1, y - 2, 3, col);
    tft.drawFastHLine(x - 2, y - 1, 5, col);
    tft.drawFastHLine(x - 4, y, 9, col);
    tft.drawFastHLine(x - 2, y + 1, 5, col);
    tft.drawFastHLine(x - 1, y + 2, 3, col);
    tft.drawPixel(x, y + 3, col);
    tft.drawPixel(x, y + 4, col);
}

void FridayFace::eraseHeart(int x, int y, int s) {
    DisplayManager::tft.fillRect(x - s * 2 - 1, y - s - 1, s * 4 + 2, s * 3 + 2, HUD_DARK);
}

void FridayFace::eraseSparkle(int x, int y, int sz) {
    DisplayManager::tft.fillRect(x - sz - 1, y - sz - 1, sz * 2 + 3, sz * 2 + 3, HUD_DARK);
}

// ─── Animation Sequences ───────────────────────────

void FridayFace::playEyeOpen() {
    auto& tft = DisplayManager::tft;

    drawFullScene();
    drawBlush();

    for (int h = 4; h <= EYE_H; h += 3) {
        clearVisor();
        drawBlush();
        int r = min(h / 3, (int)EYE_R);
        tft.fillRoundRect(ELX - EYE_W / 2 - 3, EYY - h / 2 - 3,
                          EYE_W + 6, h + 6, r + 2, CHAR_EYE_DIM);
        tft.fillRoundRect(ERX - EYE_W / 2 - 3, EYY - h / 2 - 3,
                          EYE_W + 6, h + 6, r + 2, CHAR_EYE_DIM);
        tft.fillRoundRect(ELX - EYE_W / 2, EYY - h / 2, EYE_W, h, r, CHAR_EYE_GLOW);
        tft.fillRoundRect(ERX - EYE_W / 2, EYY - h / 2, EYE_W, h, r, CHAR_EYE_GLOW);
        delay(50);
    }

    clearVisor();
    drawEyes(0, 0, false);
    drawBlush();
    prevEyeOX = 0; prevEyeOY = 0; prevBlink = false;
    delay(700);

    // Double blink
    updateEyes(0, 0, true);  delay(90);
    updateEyes(0, 0, false); delay(140);
    updateEyes(0, 0, true);  delay(90);
    updateEyes(0, 0, false); delay(400);
}

void FridayFace::playIdle(unsigned long durationMs) {
    drawFullScene();
    drawBlush();
    prevEyeOX = -99; // Force first draw

    unsigned long start = millis();
    while (millis() - start < durationMs) {
        updateEyes(0, 0, false);  delay(500);
        updateEyes(0, 0, true);   delay(100);
        updateEyes(0, 0, false);  delay(300);
        updateEyes(-6, -2, false); delay(500);
        updateEyes(0, 0, true);   delay(100);
        updateEyes(6, 2, false);  delay(500);
        updateEyes(0, 0, false);  delay(250);
        // Triple blink
        updateEyes(0, 0, true);  delay(70);
        updateEyes(0, 0, false); delay(100);
        updateEyes(0, 0, true);  delay(70);
        updateEyes(0, 0, false); delay(100);
        updateEyes(0, 0, true);  delay(70);
        updateEyes(0, 0, false); delay(400);
    }
}

void FridayFace::playLove(unsigned long durationMs) {
    clearVisor();
    drawFullScene();
    drawStar(305, 90, HUD_WHITE);
    drawStar(15, 80, HUD_WHITE);
    prevHeartsDrawn = false;
    prevSparklesDrawn = false;

    unsigned long start = millis();
    int f = 0;
    while (millis() - start < durationMs) {
        if (prevHeartsDrawn) {
            eraseHeart(prevH1x, prevH1y, 4);
            eraseHeart(prevH2x, prevH2y, 3);
            eraseHeart(prevH3x, prevH3y, 3);
            eraseHeart(prevH4x, prevH4y, 4);
        }
        if (prevSparklesDrawn) {
            eraseSparkle(prevSp1x, prevSp1y, 4);
            eraseSparkle(prevSp2x, prevSp2y, 3);
        }

        int pulse = abs((f % 16) - 8) / 3;
        clearVisor();
        drawHeartEyes(pulse);
        drawBlush();

        int h1y = 50 - (f * 3) % 30;
        int h2y = 65 - (f * 2) % 25;
        int h3y = 120 - (f * 2) % 20;
        int h4y = 45 - (f * 4) % 35;
        drawHeart(45, h1y, 4);
        drawHeart(275, h2y, 3);
        drawHeart(35, h3y, 3);
        drawHeart(285, h4y, 4);
        prevH1x = 45; prevH1y = h1y;
        prevH2x = 275; prevH2y = h2y;
        prevH3x = 35; prevH3y = h3y;
        prevH4x = 285; prevH4y = h4y;
        prevHeartsDrawn = true;

        int sp1x, sp1y, sp2x, sp2y;
        if (f % 4 < 2) {
            sp1x = 55; sp1y = 40; sp2x = 290; sp2y = 130;
        } else {
            sp1x = 265; sp1y = 35; sp2x = 30; sp2y = 140;
        }
        drawSparkle(sp1x, sp1y, 4, HUD_YELLOW);
        drawSparkle(sp2x, sp2y, 3, HUD_YELLOW);
        prevSp1x = sp1x; prevSp1y = sp1y;
        prevSp2x = sp2x; prevSp2y = sp2y;
        prevSparklesDrawn = true;

        drawStar(305, 90, HUD_WHITE);
        drawStar(15, 80, HUD_WHITE);

        delay(130);
        f++;
    }
}

void FridayFace::playSurprised(unsigned long durationMs) {
    auto& tft = DisplayManager::tft;
    drawFullScene();

    drawSparkle(50, 45, 5, HUD_YELLOW);
    drawSparkle(270, 40, 5, HUD_YELLOW);
    drawSparkle(45, 140, 4, HUD_YELLOW);
    drawSparkle(280, 145, 4, HUD_YELLOW);
    drawStar(30, 80, HUD_WHITE);
    drawStar(295, 70, HUD_WHITE);

    clearVisor();
    drawBigEyes();
    drawBlush();
    delay(500);

    // Head shake
    for (int i = 0; i < 4; i++) {
        tft.fillCircle(HCX, HCY, H_OUT + 4, HUD_DARK);
        tft.fillCircle(HCX + 3, HCY, H_OUT, CHAR_HELMET);
        tft.fillCircle(HCX + 3, HCY, H_IN, CHAR_VISOR);
        tft.fillCircle(HCX + 3 - H_OUT + 4, HCY + 6, 11, CHAR_HELMET);
        tft.fillCircle(HCX + 3 + H_OUT - 4, HCY + 6, 11, CHAR_HELMET);
        drawBigEyes(); drawBlush();
        delay(80);

        tft.fillCircle(HCX + 3, HCY, H_OUT + 1, HUD_DARK);
        tft.fillCircle(HCX - 3, HCY, H_OUT, CHAR_HELMET);
        tft.fillCircle(HCX - 3, HCY, H_IN, CHAR_VISOR);
        tft.fillCircle(HCX - 3 - H_OUT + 4, HCY + 6, 11, CHAR_HELMET);
        tft.fillCircle(HCX - 3 + H_OUT - 4, HCY + 6, 11, CHAR_HELMET);
        drawBigEyes(); drawBlush();
        delay(80);
    }

    // Settle
    tft.fillCircle(HCX - 3, HCY, H_OUT + 1, HUD_DARK);
    drawHelmet();
    clearVisor();
    drawBigEyes();
    drawBlush();

    drawSparkle(50, 45, 5, HUD_YELLOW);
    drawSparkle(270, 40, 5, HUD_YELLOW);
    drawStar(30, 80, HUD_WHITE);
    drawStar(295, 70, HUD_WHITE);
    delay(1000);

    // Blink and relax
    clearVisor(); drawEyes(0, 0, true); drawBlush(); delay(100);
    clearVisor(); drawEyes(0, 0, false); drawBlush(); delay(500);
    clearVisor(); drawBigEyes(); drawBlush(); delay(800);
    clearVisor(); drawEyes(0, 0, true); drawBlush(); delay(100);
    clearVisor(); drawEyes(0, 0, false); drawBlush(); delay(400);
}
