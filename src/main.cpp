#include <Arduino.h>
#include "config.h"
#include "display/display_manager.h"
#include "display/animations.h"
#include "display/ui_theme.h"
#include "friday_core/input_manager.h"
#include "friday_core/state_machine.h"
#include "menu/radial_menu.h"
#include "utils/wifi_manager.h"
#include "utils/time_utils.h"

// ═══════════════════════════════════════════════════
//        ★  F R I D A Y  ★
//        Your Favorite DeskBuddy
//
//  ESP32-based smart desktop assistant
//  Iron Man HUD-inspired UI
//  Version 1.0
// ═══════════════════════════════════════════════════

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("═══════════════════════════════════════");
    Serial.println("  FRIDAY — Your Favorite DeskBuddy");
    Serial.println("  Version 1.0");
    Serial.println("═══════════════════════════════════════");

    // ─── Initialize Display ────────────────────────
    Serial.println("[INIT] Display...");
    DisplayManager::init();

    // ─── Play Cinematic FRIDAY Boot Sequence ───────
    // This sequence handles all hardware initializations
    // (Input, WiFi, Time, StateMachine) internally while
    // rendering the futuristic boot animations.
    Animations::playCinematicBoot();

    Serial.println("[READY] FRIDAY is online!");
    Serial.printf("[MEM] Free heap: %d bytes\n", ESP.getFreeHeap());
}

void loop() {
    // ─── 1. Poll Inputs ────────────────────────────
    InputManager::poll();
    InputEvent event = InputManager::getEvent();

    // ─── 2. Handle Input ───────────────────────────
    StateMachine::handleInput(event);

    // ─── 3. Update Logic ───────────────────────────
    StateMachine::update();

    // ─── 4. Render Display ─────────────────────────
    StateMachine::draw();

    // ─── 5. Background Tasks ───────────────────────
    // Periodic WiFi reconnect check
    static unsigned long lastWiFiCheck = 0;
    if (millis() - lastWiFiCheck > 30000) {
        WiFiManager::reconnectIfNeeded();
        lastWiFiCheck = millis();
    }

    // Periodic NTP sync
    TimeUtils::update();

    // Small yield to prevent watchdog timeout
    yield();
}
