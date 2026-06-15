#include "display_manager.h"
#include "iron_man_sprites.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Display Manager Implementation
// ═══════════════════════════════════════════════════

Adafruit_ILI9341 DisplayManager::tft(TFT_CS, TFT_DC, TFT_RST);

void DisplayManager::init() {
    tft.begin(TFT_SPI_FREQ);
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(HUD_DARK);
}

// ─── Text Helpers ──────────────────────────────────

void DisplayManager::drawCenteredText(const char* text, int y, uint8_t size, uint16_t color) {
    tft.setTextSize(size);
    tft.setTextColor(color);
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    tft.setCursor((SCREEN_W - w) / 2, y);
    tft.print(text);
}

void DisplayManager::drawRightText(const char* text, int x, int y, uint8_t size, uint16_t color) {
    tft.setTextSize(size);
    tft.setTextColor(color);
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    tft.setCursor(x - w, y);
    tft.print(text);
}

void DisplayManager::drawText(const char* text, int x, int y, uint8_t size, uint16_t color) {
    tft.setTextSize(size);
    tft.setTextColor(color);
    tft.setCursor(x, y);
    tft.print(text);
}

// ─── Shape Helpers ─────────────────────────────────

void DisplayManager::drawArc(int cx, int cy, int r, float startAngle, float endAngle,
                             uint16_t color, int thickness) {
    float step = 1.0f / (float)r;  // Adaptive step based on radius
    for (float a = startAngle; a <= endAngle; a += step) {
        float rad = a * DEG_TO_RAD;
        float cosA = cos(rad);
        float sinA = sin(rad);
        for (int t = 0; t < thickness; t++) {
            int px = cx + (int)((r - t) * cosA);
            int py = cy + (int)((r - t) * sinA);
            tft.drawPixel(px, py, color);
        }
    }
}

void DisplayManager::drawGlowCircle(int cx, int cy, int r, uint16_t coreColor,
                                    uint16_t glowColor, int glowSize) {
    // Outer glow
    tft.fillCircle(cx, cy, r + glowSize, glowColor);
    // Core
    tft.fillCircle(cx, cy, r, coreColor);
}

void DisplayManager::drawProgressBar(int x, int y, int w, int h, float progress,
                                     uint16_t fillColor, uint16_t bgColor) {
    // Background
    tft.fillRoundRect(x, y, w, h, h / 2, bgColor);
    // Fill
    int fillW = (int)(w * constrain(progress, 0.0f, 1.0f));
    if (fillW > 0) {
        tft.fillRoundRect(x, y, fillW, h, h / 2, fillColor);
    }
    // Border
    tft.drawRoundRect(x, y, w, h, h / 2, fillColor);
}

void DisplayManager::drawHUDFrame(int x, int y, int w, int h, uint16_t color) {
    int cornerLen = 10;

    // Top-left corner
    tft.drawFastHLine(x, y, cornerLen, color);
    tft.drawFastVLine(x, y, cornerLen, color);

    // Top-right corner
    tft.drawFastHLine(x + w - cornerLen, y, cornerLen, color);
    tft.drawFastVLine(x + w - 1, y, cornerLen, color);

    // Bottom-left corner
    tft.drawFastHLine(x, y + h - 1, cornerLen, color);
    tft.drawFastVLine(x, y + h - cornerLen, cornerLen, color);

    // Bottom-right corner
    tft.drawFastHLine(x + w - cornerLen, y + h - 1, cornerLen, color);
    tft.drawFastVLine(x + w - 1, y + h - cornerLen, cornerLen, color);
}

void DisplayManager::drawDashedHLine(int x, int y, int w, uint16_t color, int dashLen) {
    for (int i = 0; i < w; i += dashLen * 2) {
        int len = min(dashLen, w - i);
        tft.drawFastHLine(x + i, y, len, color);
    }
}

void DisplayManager::drawDashedVLine(int x, int y, int h, uint16_t color, int dashLen) {
    for (int i = 0; i < h; i += dashLen * 2) {
        int len = min(dashLen, h - i);
        tft.drawFastVLine(x, y + i, len, color);
    }
}

// ─── Screen Helpers ────────────────────────────────

void DisplayManager::clearScreen() {
    tft.fillScreen(HUD_DARK);
}

void DisplayManager::drawHeader(const char* title) {
    // Header background bar
    tft.fillRect(0, 0, SCREEN_W, HEADER_HEIGHT, HUD_DARK_BLUE);
    tft.drawFastHLine(0, HEADER_HEIGHT - 1, SCREEN_W, HUD_CYAN);

    // Draw < BACK button on the left (large touch target)
    drawBackButton(false);

    // Title centered in header
    drawCenteredText(title, 8, FONT_MEDIUM, HUD_CYAN);

    // HUD corner decorations (right side only, left has back button)
    tft.drawFastHLine(SCREEN_W - 15, 0, 15, HUD_DIM_CYAN);
    tft.drawFastVLine(SCREEN_W - 1, 0, 10, HUD_DIM_CYAN);
}

void DisplayManager::fillArea(int x, int y, int w, int h, uint16_t color) {
    tft.fillRect(x, y, w, h, color);
}

// ─── Back Button ───────────────────────────────────
// Consistent, large-target back button for every app header.

void DisplayManager::drawBackButton(bool pressed) {
    if (pressed) {
        // Pressed/active state — inverted colors for visual feedback
        tft.fillRoundRect(2, 2, BACK_BTN_W - 4, HEADER_HEIGHT - 4, 4, HUD_CYAN);
        tft.drawRoundRect(2, 2, BACK_BTN_W - 4, HEADER_HEIGHT - 4, 4, HUD_BRIGHT_CYAN);
        tft.setTextSize(FONT_SMALL);
        tft.setTextColor(HUD_BLACK);
        tft.setCursor(10, (HEADER_HEIGHT - 8) / 2);
        tft.print("< BACK");
    } else {
        // Normal state
        tft.fillRoundRect(2, 2, BACK_BTN_W - 4, HEADER_HEIGHT - 4, 4, HUD_DARK);
        tft.drawRoundRect(2, 2, BACK_BTN_W - 4, HEADER_HEIGHT - 4, 4, HUD_CYAN);
        tft.setTextSize(FONT_SMALL);
        tft.setTextColor(HUD_CYAN);
        tft.setCursor(10, (HEADER_HEIGHT - 8) / 2);
        tft.print("< BACK");
    }
}

// ─── Touch Feedback ────────────────────────────────

void DisplayManager::drawTouchRipple(int x, int y, uint16_t color) {
    // Brief expanding circle ripple — sci-fi HUD effect
    for (int r = 2; r <= 12; r += 3) {
        tft.drawCircle(x, y, r, color);
    }
    delay(TOUCH_FEEDBACK_MS / 2);
    // Erase the ripple by redrawing in background color
    for (int r = 2; r <= 12; r += 3) {
        tft.drawCircle(x, y, r, HUD_DARK);
    }
}

void DisplayManager::flashButton(int x, int y, int w, int h,
                                  const char* label, uint16_t color) {
    // Show pressed state
    drawTouchButtonActive(x, y, w, h, label, color);
    delay(TOUCH_FEEDBACK_MS);
    // Restore normal state
    drawTouchButton(x, y, w, h, label, color);
}

// ─── Transition Effects ────────────────────────────

void DisplayManager::drawIronManFigure(int x, int y, int scale, IronManState state) {
    const uint16_t* sprite;
    
    switch (state) {
        case IronManState::DANCING:     sprite = ironManDancing; break;
        case IronManState::FLYING:      sprite = ironManFlying; break;
        case IronManState::LOVE:        sprite = ironManLove; break;
        case IronManState::SAD:         sprite = ironManSad; break;
        case IronManState::HEARTBROKEN: sprite = ironManHeartbroken; break;
        case IronManState::HAPPY:       sprite = ironManHappy; break;
        case IronManState::SHOOTING:    sprite = ironManShooting; break;
        case IronManState::STANDING:
        default:                        sprite = ironManStanding; break;
    }
    
    // Draw the 16x24 sprite with scaling
    for (int sy = 0; sy < 24; sy++) {
        for (int sx = 0; sx < 16; sx++) {
            // Read from PROGMEM (though ESP32 maps it transparently, it's good practice)
            uint16_t color = pgm_read_word(&sprite[sy * 16 + sx]);
            
            // Treat 0x0000 as transparent
            if (color != 0x0000) {
                if (scale == 1) {
                    tft.drawPixel(x + sx, y + sy, color);
                } else {
                    tft.fillRect(x + sx * scale, y + sy * scale, scale, scale, color);
                }
            }
        }
    }
}

void DisplayManager::ironManFlightTransition(uint16_t bgColor) {
    int startX = -30;
    int startY = SCREEN_H + 30;
    int endX = SCREEN_W + 30;
    int endY = -30;
    
    // Clear screen first to provide a clean slate for the animation
    tft.fillScreen(HUD_DARK);

    // Only draw Iron Man in 4 distinct positions across the screen
    int steps = 4; 
    for (int i = 0; i <= steps; i++) {
        // Calculate position
        int x = startX + (endX - startX) * i / steps;
        int y = startY + (endY - startY) * i / steps;

        // Draw thruster trail
        if (i > 0) {
            int px = startX + (endX - startX) * (i - 1) / steps;
            int py = startY + (endY - startY) * (i - 1) / steps;
            tft.fillCircle(px - 4, py + 16, 4, IRON_GOLD);
            tft.fillCircle(px + 4, py + 16, 4, IRON_GOLD);
            tft.fillCircle(px, py + 20, 6, IRON_RED);
        }

        // Draw Iron Man
        drawIronManFigure(x, y, 1);
        
        // Slow down the animation significantly
        delay(120);
        
        // Erase old figure completely so they don't stack up into a band!
        // We clear a large enough rectangle around him to erase him but leave some trail
        tft.fillRect(x - 15, y - 15, 30, 35, HUD_DARK);
    }
    
    // Clear screen fully to the target background color
    tft.fillScreen(bgColor);
}

void DisplayManager::irisOpen(uint16_t color) {
    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;
    int maxR = 200; // Enough to cover corners

    for (int r = 0; r <= maxR; r += 8) {
        tft.fillCircle(cx, cy, r, color);
        // Draw cyan ring at leading edge for sci-fi effect
        if (r > 4) {
            tft.drawCircle(cx, cy, r, HUD_CYAN);
            tft.drawCircle(cx, cy, r - 1, HUD_DIM_CYAN);
        }
    }
    tft.fillScreen(color);
}

void DisplayManager::irisClose(uint16_t color) {
    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;
    int maxR = 200;

    // Start with cyan ring closing inward
    for (int r = maxR; r >= 0; r -= 6) {
        // Fill outside the circle with the target color
        // Approximate by drawing larger filled circle in bg, then content circle
        tft.drawCircle(cx, cy, r + 2, color);
        tft.drawCircle(cx, cy, r + 1, color);
        tft.drawCircle(cx, cy, r, HUD_CYAN);
    }
    tft.fillScreen(color);
}

void DisplayManager::scanWipe(uint16_t color) {
    for (int y = 0; y < SCREEN_H; y += 4) {
        tft.drawFastHLine(0, y, SCREEN_W, HUD_CYAN);
        tft.fillRect(0, y, SCREEN_W, 4, color);
        // Small delay is intentional for visual effect during transitions
    }
}

// ─── Touch Button Helpers ──────────────────────────

void DisplayManager::drawTouchButton(int x, int y, int w, int h,
                                      const char* label, uint16_t color,
                                      uint16_t textColor) {
    tft.fillRoundRect(x, y, w, h, CORNER_R, HUD_DARK_BLUE);
    tft.drawRoundRect(x, y, w, h, CORNER_R, color);

    // Center text in button
    tft.setTextSize(FONT_SMALL);
    int16_t x1, y1;
    uint16_t tw, th;
    tft.getTextBounds(label, 0, 0, &x1, &y1, &tw, &th);
    tft.setCursor(x + (w - tw) / 2, y + (h - th) / 2);
    tft.setTextColor(textColor);
    tft.print(label);
}

void DisplayManager::drawTouchButtonActive(int x, int y, int w, int h,
                                            const char* label, uint16_t color) {
    tft.fillRoundRect(x, y, w, h, CORNER_R, color);
    tft.drawRoundRect(x, y, w, h, CORNER_R, HUD_BRIGHT_CYAN);

    // Center text in button (inverted colors)
    tft.setTextSize(FONT_SMALL);
    int16_t x1, y1;
    uint16_t tw, th;
    tft.getTextBounds(label, 0, 0, &x1, &y1, &tw, &th);
    tft.setCursor(x + (w - tw) / 2, y + (h - th) / 2);
    tft.setTextColor(HUD_BLACK);
    tft.print(label);
}

