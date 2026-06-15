#include "deskbuddy_app.h"
#include "../utils/time_utils.h"
#include "../config.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — DeskBuddy Mode Implementation (Touch)
//  Idle AI companion with personality
// ═══════════════════════════════════════════════════

// Motivational, funny, and engineering quotes stored in PROGMEM
const char* DeskBuddyApp::quotes[QUOTE_COUNT] = {
    "Coffee levels appear critical.",
    "Assignment deadline detected.",
    "Productivity probability: 12%.",
    "You've been staring for 47 min.",
    "Deploying sarcasm module...",
    "Running on caffeine & hope.",
    "Error 404: Motivation not found.",
    "Have you tried off and on again?",
    "Your code has fewer bugs. Suspicious.",
    "I'm not saying aliens... but aliens.",
    "Remember to hydrate, human.",
    "Compiling... just kidding.",
    "Segfault in motivation.cpp line 42.",
    "sudo make me a sandwich.",
    "Keep going, you're doing great!"
};

void DeskBuddyApp::onEnter() {
    currentQuote = 0;
    lastQuoteTime = millis();
    lastAnimTime = millis();
    animPhase = 0;
    faceDrawn = false;

    // Draw the FRIDAY face
    FridayFace::drawFullScene();
    FridayFace::drawBlush();
    FridayFace::drawEyes(0, 0, false);
    faceDrawn = true;

    // Brief interaction hint on first entry
    DisplayManager::drawCenteredText("Tap: Quote  Swipe: Expression", SCREEN_H - 12,
                                     FONT_TINY, HUD_DIM_CYAN);
}

void DeskBuddyApp::onExit() {
    // Nothing
}

void DeskBuddyApp::onInput(InputEvent event) {
    if (event.type == InputType::TOUCH_TAP) {
        // Tap on screen → next quote
        nextQuote();
    }

    if (event.type == InputType::TOUCH_SWIPE_LEFT ||
        event.type == InputType::TOUCH_SWIPE_UP) {
        // Swipe left/up → change expression forward
        animPhase = (animPhase + 1) % 3;
        FridayFace::clearVisor();
        switch (animPhase) {
            case 0:
                FridayFace::drawEyes(0, 0, false);
                break;
            case 1:
                FridayFace::drawHeartEyes(0);
                break;
            case 2:
                FridayFace::drawBigEyes();
                break;
        }
        FridayFace::drawBlush();
    }

    if (event.type == InputType::TOUCH_SWIPE_DOWN) {
        // Swipe down → change expression backward
        animPhase = (animPhase - 1 + 3) % 3;
        FridayFace::clearVisor();
        switch (animPhase) {
            case 0:
                FridayFace::drawEyes(0, 0, false);
                break;
            case 1:
                FridayFace::drawHeartEyes(0);
                break;
            case 2:
                FridayFace::drawBigEyes();
                break;
        }
        FridayFace::drawBlush();
    }
}

void DeskBuddyApp::update() {
    unsigned long now = millis();

    // Rotate quotes
    if (now - lastQuoteTime > QUOTE_INTERVAL) {
        nextQuote();
        lastQuoteTime = now;
    }

    // Simple idle animation: occasional blink
    if (animPhase == 0 && now - lastAnimTime > 3000) {
        // Blink
        FridayFace::updateEyes(0, 0, true);
        delay(80);
        FridayFace::updateEyes(0, 0, false);
        lastAnimTime = now;
    }
}

void DeskBuddyApp::draw() {
    drawOverlay();
}

void DeskBuddyApp::drawOverlay() {
    auto& tft = DisplayManager::tft;

    // Time display at top (overlaid on background)
    tft.fillRect(0, 0, SCREEN_W, 18, HUD_DARK);
    const char* timeStr = TimeUtils::getTimeShort();
    DisplayManager::drawCenteredText(timeStr, 2, FONT_MEDIUM, HUD_CYAN);

    // Quote at bottom
    tft.fillRect(0, SCREEN_H - 22, SCREEN_W, 22, HUD_DARK);
    tft.drawFastHLine(10, SCREEN_H - 22, SCREEN_W - 20, HUD_DIM_CYAN);

    // Draw the quote text (may need to truncate for screen width)
    const char* quote = quotes[currentQuote];

    // Center the quote text
    tft.setTextSize(FONT_SMALL);
    tft.setTextColor(HUD_LIGHT_GRAY);
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(quote, 0, 0, &x1, &y1, &w, &h);
    int qx = max(4, (int)(SCREEN_W - w) / 2);
    tft.setCursor(qx, SCREEN_H - 14);
    tft.print(quote);

    // Quote indicator dots
    int dotY = SCREEN_H - 4;
    for (int i = 0; i < min(QUOTE_COUNT, 7); i++) {
        int dx = SCREEN_W / 2 - 42 + i * 14;
        if (i == currentQuote % 7) {
            tft.fillCircle(dx, dotY, 2, HUD_CYAN);
        } else {
            tft.drawPixel(dx, dotY, HUD_DIM_CYAN);
        }
    }
}

void DeskBuddyApp::nextQuote() {
    currentQuote = (currentQuote + 1) % QUOTE_COUNT;
}
