#include "touch_debug_app.h"
#include <stdio.h>
#include <math.h>

void TouchDebugApp::onEnter() {
    ironManX = SCREEN_W / 2 - 8; // Center
    ironManY = SCREEN_H / 2 - 12;
    targetX = ironManX;
    targetY = ironManY;
    
    currentState = IronManState::STANDING;
    stateTimer = millis();
    
    isTouching = false;
    forceRedraw = true;

    lastTargetX = targetX;
    lastTargetY = targetY;
    lastMoveTime = millis();

    DisplayManager::clearScreen();
    DisplayManager::drawHeader("IRON MAN PET");
}

void TouchDebugApp::onExit() {
    // Nothing to clean up
}

void TouchDebugApp::update() {
    bool touched = InputManager::ts.touched();
    unsigned long now = millis();
    
    // Smooth movement (lerp)
    if (abs(ironManX - targetX) > 1.0f || abs(ironManY - targetY) > 1.0f) {
        ironManX += (targetX - ironManX) * 0.15f;
        ironManY += (targetY - ironManY) * 0.15f;
        forceRedraw = true;
    }

    if (touched) {
        TS_Point p = InputManager::ts.getPoint();
        int sx, sy;
        InputManager::mapToScreen(p.x, p.y, sx, sy);
        
        // Offset so finger doesn't completely block him
        int newTargetX = sx - 8;
        int newTargetY = sy - 30;

        // Calculate speed to detect aggressive dragging
        if (now - lastMoveTime > 50) {
            float dist = sqrt(pow(newTargetX - lastTargetX, 2) + pow(newTargetY - lastTargetY, 2));
            if (dist > 30.0f) {
                // Aggressive drag!
                currentState = IronManState::HEARTBROKEN;
                stateTimer = now + 1000;
            } else if (currentState == IronManState::HEARTBROKEN && now > stateTimer) {
                currentState = IronManState::FLYING;
            } else if (currentState != IronManState::HEARTBROKEN) {
                currentState = IronManState::FLYING;
            }
            
            lastTargetX = newTargetX;
            lastTargetY = newTargetY;
            lastMoveTime = now;
        }
        
        targetX = newTargetX;
        targetY = newTargetY;
        isTouching = true;
        forceRedraw = true;
        stateTimer = now + 1000; // Reset idle timer
    } else {
        if (isTouching) {
            isTouching = false;
            // Stop flying when touch released, revert to standing after a moment
            if (currentState == IronManState::FLYING) {
                currentState = IronManState::STANDING;
            }
            forceRedraw = true;
        }

        // Idle state cycling
        if (now > stateTimer) {
            if (currentState == IronManState::STANDING) {
                // 10% chance to look happy, 10% love, 80% just stand
                int r = random(100);
                if (r < 10) currentState = IronManState::LOVE;
                else if (r < 20) currentState = IronManState::HAPPY;
                stateTimer = now + 1500;
                forceRedraw = true;
            } else {
                currentState = IronManState::STANDING;
                stateTimer = now + 3000;
                forceRedraw = true;
            }
        }
    }
}

void TouchDebugApp::onInput(InputEvent event) {
    if (event.type == InputType::TOUCH_TAP && event.touchY > HEADER_HEIGHT) {
        // Check if he was tapped directly
        if (abs(event.touchX - ironManX - 8) < 25 && abs(event.touchY - ironManY - 12) < 25) {
            // Tickled!
            currentState = IronManState::DANCING;
            stateTimer = millis() + 2000;
        } else {
            // Tapped elsewhere - SHOOT!
            currentState = IronManState::SHOOTING;
            stateTimer = millis() + 500;
            
            // Draw a quick repulsor blast
            DisplayManager::tft.fillCircle(event.touchX, event.touchY, 10, HUD_BRIGHT_CYAN);
            delay(100);
            DisplayManager::tft.fillCircle(event.touchX, event.touchY, 15, HUD_DARK);
        }
        forceRedraw = true;
    }
}

void TouchDebugApp::draw() {
    if (!forceRedraw) return;
    forceRedraw = false;

    // Erase background (just a large rect instead of full screen to avoid flicker)
    DisplayManager::tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, HUD_DARK);

    // Draw hints
    DisplayManager::drawCenteredText("Drag to Fly | Tap Him to Tickle | Tap Screen to Shoot", HEADER_HEIGHT + 10, FONT_TINY, HUD_GRAY);

    // Draw Iron Man
    DisplayManager::drawIronManFigure((int)ironManX, (int)ironManY, 2, currentState);
}
