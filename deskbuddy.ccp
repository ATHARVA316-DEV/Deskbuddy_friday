#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// ═══════════════════════════════════════════════════
//        ★  F R I D A Y  ★  your desk buddy
// ═══════════════════════════════════════════════════

// ─── Colour palette ────────────────────────────────
#define BG_COLOR       0x0841
#define HELMET_RIM     0xE71C
#define HELMET_HI      0xFFFF
#define VISOR_BG       0x10A2
#define EYE_GLOW       0x2E1F
#define EYE_DIM        0x1C1A
#define EYE_HI         0xFFFF
#define BODY_COL       0xE71C
#define BODY_SHAD      0xB596
#define HEART_COL      0xF800
#define HEART_HI       0xFBCF
#define BLUSH_COL      0xFB2C
#define NAME_COL       0x2E1F
#define SPARKLE_COL    0xFFE0
#define STAR_COL       0xFFFF

// ─── Layout ────────────────────────────────────────
#define HCX  160
#define HCY  100
#define H_OUT  80
#define H_IN   65

#define EYE_W   32
#define EYE_H   40
#define EYE_R   13
#define ELX  130
#define ERX  190
#define EYY  97

#define BODY_Y  180
#define BODY_W  94
#define BODY_H  48

// ─── Previous state for flicker-free erase ─────────
int prevEyeOX = 0, prevEyeOY = 0;
bool prevBlink = false;

// track previous heart positions for erase
int prevH1x = 0, prevH1y = 0;
int prevH2x = 0, prevH2y = 0;
int prevH3x = 0, prevH3y = 0;
int prevH4x = 0, prevH4y = 0;
bool prevHeartsDrawn = false;

// track previous sparkle positions
int prevSp1x = 0, prevSp1y = 0;
int prevSp2x = 0, prevSp2y = 0;
bool prevSparklesDrawn = false;

// ═══════════════════════════════════════════════════
//  DRAWING HELPERS
// ═══════════════════════════════════════════════════

// ─── Clear only the visor interior ─────────────────
void clearVisor()
{
  tft.fillCircle(HCX, HCY, H_IN, VISOR_BG);
}

// ─── Erase a small rect region with BG ─────────────
void eraseRect(int x, int y, int w, int h)
{
  tft.fillRect(x, y, w, h, BG_COLOR);
}

// ─── Erase a heart at position ─────────────────────
void eraseHeart(int x, int y, int s)
{
  tft.fillRect(x - s * 2 - 1, y - s - 1, s * 4 + 2, s * 3 + 2, BG_COLOR);
}

// ─── Erase a sparkle at position ───────────────────
void eraseSparkle(int x, int y, int sz)
{
  tft.fillRect(x - sz - 1, y - sz - 1, sz * 2 + 3, sz * 2 + 3, BG_COLOR);
}

// ─── Draw the helmet (static) ──────────────────────
void drawHelmet()
{
  tft.fillCircle(HCX, HCY, H_OUT, HELMET_RIM);
  tft.fillCircle(HCX, HCY, H_IN,  VISOR_BG);

  for (int a = 125; a < 210; a++)
  {
    float r = a * 0.01745329f;
    for (int d = 3; d <= 5; d++)
    {
      int x = HCX + (int)((H_OUT - d) * cos(r));
      int y = HCY + (int)((H_OUT - d) * sin(r));
      tft.drawPixel(x, y, HELMET_HI);
    }
  }

  tft.fillCircle(HCX - H_OUT + 4, HCY + 6, 11, HELMET_RIM);
  tft.fillCircle(HCX + H_OUT - 4, HCY + 6, 11, HELMET_RIM);
  tft.fillCircle(HCX - H_OUT + 4, HCY + 6, 5,  BODY_SHAD);
  tft.fillCircle(HCX + H_OUT - 4, HCY + 6, 5,  BODY_SHAD);
}

// ─── Draw the body (static) ───────────────────────
void drawBody()
{
  int bx = HCX - BODY_W / 2;
  tft.fillRoundRect(bx, BODY_Y, BODY_W, BODY_H, 20, BODY_COL);
  tft.fillRoundRect(bx + 4, BODY_Y + 22, BODY_W - 8, BODY_H - 20, 15, BODY_SHAD);

  tft.fillCircle(HCX - 24, BODY_Y + BODY_H - 2, 11, BODY_COL);
  tft.fillCircle(HCX + 24, BODY_Y + BODY_H - 2, 11, BODY_COL);

  tft.setTextColor(NAME_COL);
  tft.setTextSize(1);
  tft.setCursor(HCX - 17, BODY_Y + 7);
  tft.print("FRIDAY");
}

// ─── Blush ─────────────────────────────────────────
void drawBlush()
{
  tft.fillCircle(ELX - 18, EYY + 18, 7, BLUSH_COL);
  tft.fillCircle(ERX + 18, EYY + 18, 7, BLUSH_COL);
}

// ─── Eye glow halo ─────────────────────────────────
void drawGlow(int cx, int cy, int w, int h)
{
  tft.fillRoundRect(cx - w / 2 - 4, cy - h / 2 - 4,
                    w + 8, h + 8, EYE_R + 3, EYE_DIM);
}

// ─── Normal eyes ───────────────────────────────────
void drawEyes(int ox, int oy, bool blink)
{
  int lx = ELX + ox;
  int rx = ERX + ox;
  int ey = EYY + oy;

  if (blink)
  {
    for (int i = -13; i <= 13; i++)
    {
      int c = -(i * i) / 12;
      tft.drawPixel(lx + i, ey + c,     EYE_GLOW);
      tft.drawPixel(lx + i, ey + c + 1, EYE_GLOW);
      tft.drawPixel(lx + i, ey + c + 2, EYE_GLOW);
      tft.drawPixel(rx + i, ey + c,     EYE_GLOW);
      tft.drawPixel(rx + i, ey + c + 1, EYE_GLOW);
      tft.drawPixel(rx + i, ey + c + 2, EYE_GLOW);
    }
    return;
  }

  drawGlow(lx, ey, EYE_W, EYE_H);
  drawGlow(rx, ey, EYE_W, EYE_H);

  tft.fillRoundRect(lx - EYE_W / 2, ey - EYE_H / 2, EYE_W, EYE_H, EYE_R, EYE_GLOW);
  tft.fillRoundRect(rx - EYE_W / 2, ey - EYE_H / 2, EYE_W, EYE_H, EYE_R, EYE_GLOW);

  tft.fillCircle(lx - 7, ey - 10, 5, EYE_HI);
  tft.fillCircle(rx - 7, ey - 10, 5, EYE_HI);

  tft.fillCircle(lx + 6, ey + 7, 2, EYE_HI);
  tft.fillCircle(rx + 6, ey + 7, 2, EYE_HI);

  tft.fillCircle(lx - 2, ey - 14, 2, EYE_HI);
  tft.fillCircle(rx - 2, ey - 14, 2, EYE_HI);
}

// ─── Big surprised eyes ───────────────────────────
void drawBigEyes()
{
  int lx = ELX;
  int rx = ERX;
  int ey = EYY - 3;
  int w  = EYE_W + 10;
  int h  = EYE_H + 14;
  int r  = 16;

  drawGlow(lx, ey, w, h);
  drawGlow(rx, ey, w, h);

  tft.fillRoundRect(lx - w / 2, ey - h / 2, w, h, r, EYE_GLOW);
  tft.fillRoundRect(rx - w / 2, ey - h / 2, w, h, r, EYE_GLOW);

  tft.fillCircle(lx - 9, ey - 14, 6, EYE_HI);
  tft.fillCircle(rx - 9, ey - 14, 6, EYE_HI);
  tft.fillCircle(lx + 7, ey + 8, 3, EYE_HI);
  tft.fillCircle(rx + 7, ey + 8, 3, EYE_HI);
  tft.fillCircle(lx - 3, ey - 20, 2, EYE_HI);
  tft.fillCircle(rx - 3, ey - 20, 2, EYE_HI);
}

// ─── Heart eyes ────────────────────────────────────
void drawHeartEyes(int pulse)
{
  int s = 8 + pulse;

  for (int side = 0; side < 2; side++)
  {
    int cx = (side == 0) ? ELX : ERX;
    int cy = EYY;

    tft.fillCircle(cx, cy + 3, s + 6, EYE_DIM);

    tft.fillCircle(cx - s + 2, cy - 2, s, HEART_COL);
    tft.fillCircle(cx + s - 2, cy - 2, s, HEART_COL);
    tft.fillTriangle(
      cx - s * 2 + 2, cy + 1,
      cx + s * 2 - 2, cy + 1,
      cx, cy + s * 2 + 4,
      HEART_COL
    );

    tft.fillCircle(cx - s + 3, cy - s + 2, 3, HEART_HI);
  }
}

// ─── Sparkle ───────────────────────────────────────
void drawSparkle(int x, int y, int sz, uint16_t col)
{
  tft.drawFastVLine(x, y - sz, sz * 2 + 1, col);
  tft.drawFastHLine(x - sz, y, sz * 2 + 1, col);
  for (int i = 1; i < sz; i++)
  {
    tft.drawPixel(x - i, y - i, col);
    tft.drawPixel(x + i, y - i, col);
    tft.drawPixel(x - i, y + i, col);
    tft.drawPixel(x + i, y + i, col);
  }
}

// ─── Heart ─────────────────────────────────────────
void drawHeart(int x, int y, int s)
{
  tft.fillCircle(x - s, y, s, HEART_COL);
  tft.fillCircle(x + s, y, s, HEART_COL);
  tft.fillTriangle(x - s * 2, y + s / 2,
                   x + s * 2, y + s / 2,
                   x, y + s * 3, HEART_COL);
  tft.fillCircle(x - s + 1, y - s / 2, max(1, s / 3), HEART_HI);
}

// ─── Star ──────────────────────────────────────────
void drawStar(int x, int y, uint16_t col)
{
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

// ═══════════════════════════════════════════════════
//  FLICKER-FREE UPDATES
// ═══════════════════════════════════════════════════

// Draw full static scene once
void drawFullScene()
{
  tft.fillScreen(BG_COLOR);
  drawBody();
  drawHelmet();
}

// Update only the eyes (clears visor, redraws eyes + blush)
void updateEyes(int ox, int oy, bool blink)
{
  // only redraw if something changed
  if (ox == prevEyeOX && oy == prevEyeOY && blink == prevBlink)
    return;

  clearVisor();
  drawEyes(ox, oy, blink);
  drawBlush();

  prevEyeOX = ox;
  prevEyeOY = oy;
  prevBlink = blink;
}

// ═══════════════════════════════════════════════════
//  INTRO SCREEN
// ═══════════════════════════════════════════════════

void introScreen()
{
  tft.fillScreen(BG_COLOR);

  drawSparkle(50,  60, 4, SPARKLE_COL);
  drawSparkle(270, 55, 3, SPARKLE_COL);
  drawSparkle(40,  170, 3, SPARKLE_COL);
  drawSparkle(280, 175, 4, SPARKLE_COL);
  drawStar(25,  110, STAR_COL);
  drawStar(295, 120, STAR_COL);

  tft.setTextColor(EYE_GLOW);
  tft.setTextSize(4);
  tft.setCursor(68, 80);
  tft.print("FRIDAY");

  tft.setTextColor(HELMET_RIM);
  tft.setTextSize(1);
  tft.setCursor(100, 130);
  tft.print("your desk buddy <3");

  drawHeart(50,  95, 3);
  drawHeart(270, 95, 3);

  delay(2500);

  // eye-open transition
  drawFullScene();
  drawBlush();

  for (int h = 4; h <= EYE_H; h += 3)
  {
    clearVisor();
    drawBlush();

    int r = min(h / 3, (int)EYE_R);

    tft.fillRoundRect(ELX - EYE_W / 2 - 3, EYY - h / 2 - 3,
                      EYE_W + 6, h + 6, r + 2, EYE_DIM);
    tft.fillRoundRect(ERX - EYE_W / 2 - 3, EYY - h / 2 - 3,
                      EYE_W + 6, h + 6, r + 2, EYE_DIM);
    tft.fillRoundRect(ELX - EYE_W / 2, EYY - h / 2, EYE_W, h, r, EYE_GLOW);
    tft.fillRoundRect(ERX - EYE_W / 2, EYY - h / 2, EYE_W, h, r, EYE_GLOW);

    delay(50);
  }

  // fully open
  clearVisor();
  drawEyes(0, 0, false);
  drawBlush();
  prevEyeOX = 0; prevEyeOY = 0; prevBlink = false;
  delay(700);

  // double blink
  updateEyes(0, 0, true);
  delay(90);
  updateEyes(0, 0, false);
  delay(140);
  updateEyes(0, 0, true);
  delay(90);
  updateEyes(0, 0, false);
  delay(400);
}

// ═══════════════════════════════════════════════════
//  IDLE MODE  (~4 sec)
// ═══════════════════════════════════════════════════

void idleMode()
{
  // draw static parts once
  drawFullScene();
  drawBlush();
  prevEyeOX = -99; // force first draw

  unsigned long start = millis();

  while (millis() - start < 4000)
  {
    updateEyes(0, 0, false);
    delay(500);

    updateEyes(0, 0, true);
    delay(100);

    updateEyes(0, 0, false);
    delay(300);

    // look left
    updateEyes(-6, -2, false);
    delay(500);

    updateEyes(0, 0, true);
    delay(100);

    // look right
    updateEyes(6, 2, false);
    delay(500);

    // back to centre
    updateEyes(0, 0, false);
    delay(250);

    // triple blink
    updateEyes(0, 0, true);
    delay(70);
    updateEyes(0, 0, false);
    delay(100);
    updateEyes(0, 0, true);
    delay(70);
    updateEyes(0, 0, false);
    delay(100);
    updateEyes(0, 0, true);
    delay(70);
    updateEyes(0, 0, false);
    delay(400);
  }
}

// ═══════════════════════════════════════════════════
//  LOVE MODE  (~6 sec)
// ═══════════════════════════════════════════════════

void loveMode()
{
  // draw static parts once
  drawFullScene();

  // draw static stars that don't move
  drawStar(305, 90, STAR_COL);
  drawStar(15,  80, STAR_COL);

  prevHeartsDrawn = false;
  prevSparklesDrawn = false;

  unsigned long start = millis();
  int f = 0;

  while (millis() - start < 6000)
  {
    // ── erase previous floating hearts ──
    if (prevHeartsDrawn)
    {
      eraseHeart(prevH1x, prevH1y, 4);
      eraseHeart(prevH2x, prevH2y, 3);
      eraseHeart(prevH3x, prevH3y, 3);
      eraseHeart(prevH4x, prevH4y, 4);
    }

    // ── erase previous sparkles ──
    if (prevSparklesDrawn)
    {
      eraseSparkle(prevSp1x, prevSp1y, 4);
      eraseSparkle(prevSp2x, prevSp2y, 3);
    }

    // ── update eyes (only clears visor) ──
    int pulse = abs((f % 16) - 8) / 3;
    clearVisor();
    drawHeartEyes(pulse);
    drawBlush();

    // ── draw new hearts ──
    int h1y = 50  - (f * 3) % 30;
    int h2y = 65  - (f * 2) % 25;
    int h3y = 120 - (f * 2) % 20;
    int h4y = 45  - (f * 4) % 35;

    drawHeart(45,  h1y, 4);
    drawHeart(275, h2y, 3);
    drawHeart(35,  h3y, 3);
    drawHeart(285, h4y, 4);

    prevH1x = 45;  prevH1y = h1y;
    prevH2x = 275; prevH2y = h2y;
    prevH3x = 35;  prevH3y = h3y;
    prevH4x = 285; prevH4y = h4y;
    prevHeartsDrawn = true;

    // ── draw new sparkles (alternating) ──
    int sp1x, sp1y, sp2x, sp2y;
    if (f % 4 < 2)
    {
      sp1x = 55;  sp1y = 40;
      sp2x = 290; sp2y = 130;
    }
    else
    {
      sp1x = 265; sp1y = 35;
      sp2x = 30;  sp2y = 140;
    }
    drawSparkle(sp1x, sp1y, 4, SPARKLE_COL);
    drawSparkle(sp2x, sp2y, 3, SPARKLE_COL);
    prevSp1x = sp1x; prevSp1y = sp1y;
    prevSp2x = sp2x; prevSp2y = sp2y;
    prevSparklesDrawn = true;

    // redraw static stars (might have been erased)
    drawStar(305, 90, STAR_COL);
    drawStar(15,  80, STAR_COL);

    delay(130);
    f++;
  }
}

// ═══════════════════════════════════════════════════
//  SURPRISED MODE  (~4 sec)
// ═══════════════════════════════════════════════════

void surprisedMode()
{
  // draw base once
  drawFullScene();

  // draw sparkle decorations (static for this mode)
  drawSparkle(50,  45, 5, SPARKLE_COL);
  drawSparkle(270, 40, 5, SPARKLE_COL);
  drawSparkle(45,  140, 4, SPARKLE_COL);
  drawSparkle(280, 145, 4, SPARKLE_COL);
  drawStar(30,  80, STAR_COL);
  drawStar(295, 70, STAR_COL);

  // draw big surprised eyes
  clearVisor();
  drawBigEyes();
  drawBlush();

  delay(500);

  // ── shake: only redraw helmet, not full screen ──
  for (int i = 0; i < 4; i++)
  {
    // erase helmet area and redraw offset right
    tft.fillCircle(HCX, HCY, H_OUT + 4, BG_COLOR);
    tft.fillCircle(HCX + 3, HCY, H_OUT, HELMET_RIM);
    tft.fillCircle(HCX + 3, HCY, H_IN,  VISOR_BG);
    tft.fillCircle(HCX + 3 - H_OUT + 4, HCY + 6, 11, HELMET_RIM);
    tft.fillCircle(HCX + 3 + H_OUT - 4, HCY + 6, 11, HELMET_RIM);
    drawBigEyes();
    drawBlush();
    delay(80);

    // erase and redraw offset left
    tft.fillCircle(HCX + 3, HCY, H_OUT + 1, BG_COLOR);
    tft.fillCircle(HCX - 3, HCY, H_OUT, HELMET_RIM);
    tft.fillCircle(HCX - 3, HCY, H_IN,  VISOR_BG);
    tft.fillCircle(HCX - 3 - H_OUT + 4, HCY + 6, 11, HELMET_RIM);
    tft.fillCircle(HCX - 3 + H_OUT - 4, HCY + 6, 11, HELMET_RIM);
    drawBigEyes();
    drawBlush();
    delay(80);
  }

  // settle back to centre
  tft.fillCircle(HCX - 3, HCY, H_OUT + 1, BG_COLOR);
  drawHelmet();
  clearVisor();
  drawBigEyes();
  drawBlush();

  // redraw sparkles (might have been erased by shake)
  drawSparkle(50,  45, 5, SPARKLE_COL);
  drawSparkle(270, 40, 5, SPARKLE_COL);
  drawStar(30,  80, STAR_COL);
  drawStar(295, 70, STAR_COL);

  delay(1000);

  // blink and relax
  clearVisor();
  drawEyes(0, 0, true);
  drawBlush();
  delay(100);
  clearVisor();
  drawEyes(0, 0, false);
  drawBlush();
  delay(500);

  // one more surprised look
  clearVisor();
  drawBigEyes();
  drawBlush();
  delay(800);

  // settle
  clearVisor();
  drawEyes(0, 0, true);
  drawBlush();
  delay(100);
  clearVisor();
  drawEyes(0, 0, false);
  drawBlush();
  delay(400);
}

// ═══════════════════════════════════════════════════
//  SETUP & LOOP
// ═══════════════════════════════════════════════════

void setup()
{
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(BG_COLOR);
}

void loop()
{
  introScreen();      // "FRIDAY — your desk buddy" + eye-open
  idleMode();         // ~4 sec — looking around, blinking
  loveMode();         // ~6 sec — heart eyes, floating hearts
  surprisedMode();    // ~4 sec — big eyes, head shake
}
