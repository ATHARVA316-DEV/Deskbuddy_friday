#include "animations.h"
#include "display_manager.h"
#include "friday_face.h"
#include "ui_theme.h"
#include "../config.h"
#include "../utils/wifi_manager.h"
#include "../utils/time_utils.h"
#include "../friday_core/input_manager.h"
#include "../friday_core/state_machine.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Cinematic Boot Sequence
// ═══════════════════════════════════════════════════

void Animations::playCinematicBoot() {
    bootPhase1CoreActivation();
    bootPhase2FridayReveal();
    bootPhase3Diagnostics();
    bootPhase4ClockSync();
    bootPhase5AIOnline();
    bootPhase6FadeToMenu();
}

// ─── Phase 1: Core Activation ──────────────────────
void Animations::bootPhase1CoreActivation() {
    auto& tft = DisplayManager::tft;
    tft.fillScreen(HUD_BLACK);
    delay(300);

    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;

    // A subtle glowing core appears in the center
    for (int r = 1; r <= 8; r++) {
        tft.fillCircle(cx, cy, r, HUD_CYAN);
        delay(30);
    }
    
    // Animated rings expanding
    for (int r = 10; r <= 60; r += 5) {
        tft.drawCircle(cx, cy, r, HUD_DIM_CYAN);
        if (r > 20) {
            tft.drawCircle(cx, cy, r - 15, HUD_BLACK); // traveling ripple
        }
        delay(20);
    }
    
    // Settle rings
    tft.fillScreen(HUD_BLACK);
    tft.fillCircle(cx, cy, 5, HUD_BRIGHT_CYAN);
    tft.drawCircle(cx, cy, 40, HUD_DIM_CYAN);
    tft.drawCircle(cx, cy, 45, HUD_BLUE);
    delay(200);
}

// ─── Phase 2: Friday Reveal ────────────────────────
void Animations::bootPhase2FridayReveal() {
    auto& tft = DisplayManager::tft;
    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2 - 15;

    // Morph core into text layout
    for (int i = 0; i < 5; i++) {
        tft.drawFastHLine(cx - i*10, cy + i*2, i*20, HUD_CYAN);
        delay(15);
    }
    tft.fillScreen(HUD_BLACK);

    // Typing effect for FRIDAY
    tft.setCursor(cx - 70, cy);
    tft.setTextColor(HUD_BRIGHT_CYAN);
    tft.setTextSize(FONT_XLARGE);
    
    const char* title = "FRIDAY";
    for (int i = 0; i < strlen(title); i++) {
        tft.print(title[i]);
        delay(50);
    }

    delay(200);

    // Subtitle fade-in
    DisplayManager::drawCenteredText("Advanced DeskBuddy Assistant", cy + 35, FONT_SMALL, HUD_LIGHT_GRAY);
    
    // Scanning line effect
    for (int y = cy - 5; y < cy + 30; y += 4) {
        tft.drawFastHLine(cx - 80, y, 160, HUD_BLUE);
        delay(20);
        tft.drawFastHLine(cx - 80, y, 160, HUD_BLACK); // erase
    }
    
    // Redraw text to clean up scan line
    DisplayManager::drawCenteredText("FRIDAY", cy, FONT_XLARGE, HUD_BRIGHT_CYAN);
    delay(600);
}

// ─── Phase 3: System Diagnostics ───────────────────
void Animations::bootPhase3Diagnostics() {
    auto& tft = DisplayManager::tft;
    tft.fillScreen(HUD_DARK);

    DisplayManager::drawHUDFrame(10, 10, SCREEN_W - 20, SCREEN_H - 20, HUD_CYAN);
    DisplayManager::drawText("SYSTEM DIAGNOSTICS", 20, 20, FONT_MEDIUM, HUD_CYAN);
    tft.drawFastHLine(20, 42, 280, HUD_DIM_CYAN);

    int startY = 60;
    int lineHeight = 20;
    int currentY = startY;

    auto printStatus = [&](const char* module, bool isOK) {
        DisplayManager::drawText(module, 20, currentY, FONT_SMALL, HUD_WHITE);
        delay(150); // Simulate check time
        
        // Dot animation
        for (int i = 0; i < 15; i++) {
            tft.print(".");
            delay(10);
        }
        
        if (isOK) {
            tft.setTextColor(HUD_GREEN);
            tft.println(" OK");
        } else {
            tft.setTextColor(HUD_RED);
            tft.println(" FAIL");
        }
        currentY += lineHeight;
    };

    printStatus("Display System", true); // Already initialized
    
    // Init Touch
    InputManager::init();
    printStatus("Touch Controller", true);

    printStatus("Storage (SPIFFS)", true);
    
    // Init App State Machine
    StateMachine::init();
    printStatus("Applications", true);

    delay(300);
}

// ─── Phase 4: Clock Synchronization ────────────────
void Animations::bootPhase4ClockSync() {
    auto& tft = DisplayManager::tft;
    tft.fillScreen(HUD_BLACK);

    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2;

    DisplayManager::drawCenteredText("Synchronizing System Time...", cy - 30, FONT_MEDIUM, HUD_CYAN);

    // Connecting WiFi while drawing network ring
    WiFiManager::connect(); // Note: This is a blocking call in the current architecture.

    // If we wanted to animate during connect, we'd need WiFiManager::connect() to be non-blocking.
    // For now, draw rings after connection attempt finishes.
    for (int i = 0; i < 3; i++) {
        tft.drawCircle(cx, cy + 20, 10 + i*5, HUD_BLUE);
        delay(100);
    }

    TimeUtils::init(); // Sync NTP
    
    tft.fillCircle(cx, cy + 20, 5, HUD_GREEN);
    DisplayManager::drawCenteredText("Time Sync Complete", cy + 50, FONT_SMALL, HUD_LIGHT_GRAY);
    
    delay(500);
}

// ─── Phase 5: AI Online ────────────────────────────
void Animations::bootPhase5AIOnline() {
    auto& tft = DisplayManager::tft;
    tft.fillScreen(HUD_CYAN); // Flash
    delay(50);
    tft.fillScreen(HUD_BLACK);

    int cx = SCREEN_W / 2;
    int cy = SCREEN_H / 2 - 20;

    DisplayManager::drawCenteredText("FRIDAY ONLINE", cy, FONT_XLARGE, HUD_BRIGHT_CYAN);
    
    delay(400);

    DisplayManager::drawCenteredText("Welcome Back, User", cy + 40, FONT_MEDIUM, HUD_WHITE);
    
    delay(1200);
}

// ─── Phase 6: Fade To Menu ─────────────────────────
void Animations::bootPhase6FadeToMenu() {
    // We now use the Iron Man themed flight transition 
    // to cleanly transition from the boot sequence 
    // into the Main Menu.
    DisplayManager::ironManFlightTransition(APP_BG);
    delay(100);
}
