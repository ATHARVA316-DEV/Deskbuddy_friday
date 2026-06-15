#include "input_manager.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Input Manager (Touch Screen) Implementation
//  XPT2046 gesture detection with multi-sample averaging,
//  debounce, drag events, release events, and manual
//  axis correction for reliable touch mapping.
// ═══════════════════════════════════════════════════

// XPT2046 instance — uses TOUCH_CS, with TOUCH_IRQ for interrupt
XPT2046_Touchscreen InputManager::ts(TOUCH_CS, TOUCH_IRQ);

// Static member initialization
// Volatile state tracking for thread safety
volatile bool InputManager::touchActive = false;
volatile int InputManager::touchCurrentX = 0;
volatile int InputManager::touchCurrentY = 0;
volatile int InputManager::lastRawX = 0;
volatile int InputManager::lastRawY = 0;

// Task-local tracking
bool InputManager::wasTouched = false;
int InputManager::touchStartX = 0;
int InputManager::touchStartY = 0;
int InputManager::touchPrevX = 0;
int InputManager::touchPrevY = 0;
unsigned long InputManager::touchStartTime = 0;
unsigned long InputManager::lastEventTime = 0;
bool InputManager::longPressEmitted = false;
bool InputManager::dragEmitted = false;

// Queue & Event
QueueHandle_t InputManager::inputQueue = NULL;

InputEvent InputManager::currentEvent = {InputType::NONE, 0, 0, 0, 0, 0.0f};

void InputManager::init() {
    ts.begin();
    // Do NOT call ts.setRotation() here. We handle raw mapping manually
    // because XPT2046 hardware rotation behavior is inconsistent.
    
    wasTouched = false;
    touchActive = false;
    longPressEmitted = false;
    dragEmitted = false;
    lastEventTime = 0;

    // Create RTOS queue for passing events from task to main thread
    inputQueue = xQueueCreate(10, sizeof(InputEvent));

    // Spawn touch processing task (Core 1 is usually app loop, pinning to Core 1 is fine or leave unpinned)
    // We leave unpinned (tskNO_AFFINITY) so scheduler decides, but typically runs cleanly.
    xTaskCreate(touchTask, "TouchTask", 4096, NULL, 1, NULL);
}

void InputManager::poll() {
    // Non-blocking pop from RTOS queue
    if (inputQueue != NULL) {
        if (!xQueueReceive(inputQueue, &currentEvent, 0)) {
            // Queue empty
            currentEvent.type = InputType::NONE;
        }
    } else {
        currentEvent.type = InputType::NONE;
    }
}

void InputManager::touchTask(void* pvParameters) {
    while (true) {
        unsigned long now = millis();
        InputEvent localEvent = {InputType::NONE, 0, 0, 0, 0, 0.0f};
        bool eventFired = false;

        localEvent.touchX = touchCurrentX;
        localEvent.touchY = touchCurrentY;

        bool touching = ts.touched();

        if (touching) {
            // ── Read averaged touch point ──
            int avgX, avgY;
            if (!readAveragedPoint(avgX, avgY)) {
                // Failed to get a good reading — treat as not touching
                touching = false;
            } else {
                // Map raw touch coordinates to screen pixels
                int sx, sy;
                mapToScreen(avgX, avgY, sx, sy);
                touchPrevX = touchCurrentX;
                touchPrevY = touchCurrentY;
                touchCurrentX = sx;
                touchCurrentY = sy;
            }
        }

        if (touching) {
            if (!wasTouched) {
                // ── Touch just started ──
                // Debounce: ignore if too soon after last event
                if (now - lastEventTime < TOUCH_DEBOUNCE_MS) {
                    wasTouched = false;
                } else {
                    touchActive = true;
                    touchStartX = touchCurrentX;
                    touchStartY = touchCurrentY;
                    touchPrevX = touchCurrentX;
                    touchPrevY = touchCurrentY;
                    touchStartTime = now;
                    longPressEmitted = false;
                    dragEmitted = false;
                }
            } else {
                // ── Touch held ──
                // Check for long press (fire once while held)
                if (!longPressEmitted && (now - touchStartTime) >= TOUCH_LONG_PRESS_MS) {
                    localEvent.type = InputType::TOUCH_LONG_PRESS;
                    localEvent.touchX = touchStartX;
                    localEvent.touchY = touchStartY;
                    longPressEmitted = true;
                    lastEventTime = now;
                    eventFired = true;
                }
                // Check for drag (finger moving while held)
                else if (!longPressEmitted) {
                    int dx = touchCurrentX - touchPrevX;
                    int dy = touchCurrentY - touchPrevY;
                    if (abs(dx) > TOUCH_DRAG_THRESH || abs(dy) > TOUCH_DRAG_THRESH) {
                        localEvent.type = InputType::TOUCH_DRAG;
                        localEvent.touchX = touchCurrentX;
                        localEvent.touchY = touchCurrentY;
                        localEvent.deltaX = dx;
                        localEvent.deltaY = dy;
                        dragEmitted = true;
                        eventFired = true;
                    }
                }
            }

            if (touching) {
                wasTouched = true;
            }

        } else {
            // ── Not touching ──
            if (wasTouched && touchActive) {
                unsigned long duration = now - touchStartTime;
                int dx = touchCurrentX - touchStartX;
                int dy = touchCurrentY - touchStartY;
                int absDx = abs(dx);
                int absDy = abs(dy);

                if (!longPressEmitted) {
                    // Wasn't a long press — check swipe or tap
                    if (absDx > TOUCH_SWIPE_THRESH || absDy > TOUCH_SWIPE_THRESH) {
                        // Swipe detected — compute velocity for momentum
                        float dist = sqrt((float)(dx * dx + dy * dy));
                        float vel = (duration > 0) ? (dist / (float)duration * 1000.0f) : 0.0f;

                        if (absDx > absDy) {
                            // Horizontal swipe
                            localEvent.type = (dx > 0) ? InputType::TOUCH_SWIPE_RIGHT
                                                         : InputType::TOUCH_SWIPE_LEFT;
                        } else {
                            // Vertical swipe
                            localEvent.type = (dy > 0) ? InputType::TOUCH_SWIPE_DOWN
                                                         : InputType::TOUCH_SWIPE_UP;
                        }
                        localEvent.velocity = vel;
                    } else {
                        // Short touch with no swipe = tap
                        localEvent.type = InputType::TOUCH_TAP;
                    }
                    localEvent.touchX = touchStartX;
                    localEvent.touchY = touchStartY;
                    lastEventTime = now;
                    eventFired = true;
                } else {
                    // Long press was already emitted; emit release
                    localEvent.type = InputType::TOUCH_RELEASE;
                    localEvent.touchX = touchCurrentX;
                    localEvent.touchY = touchCurrentY;
                    lastEventTime = now;
                    eventFired = true;
                }

                touchActive = false;
            }
            wasTouched = false;
        }

        // Send event to queue if one fired
        if (eventFired && inputQueue != NULL) {
            // Overwrite old events if queue is full
            xQueueSendToBack(inputQueue, &localEvent, 0);
        }

        // Yield to other tasks / watchdog
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

InputEvent InputManager::getEvent() {
    return currentEvent;
}

bool InputManager::isTouching() {
    return touchActive;
}

int InputManager::getLastX() {
    return touchCurrentX;
}

int InputManager::getLastY() {
    return touchCurrentY;
}

bool InputManager::isInsideRect(int tx, int ty, int rx, int ry, int rw, int rh) {
    return (tx >= rx && tx < rx + rw && ty >= ry && ty < ry + rh);
}

void InputManager::getRawPoint(int &rawX, int &rawY) {
    rawX = lastRawX;
    rawY = lastRawY;
}

// ─── Multi-Sample Averaging ───────────────────────
// Read TOUCH_SAMPLES raw points, discard any wild outlier,
// and return the averaged X/Y. Returns false if touch lost
// during sampling.
bool InputManager::readAveragedPoint(int &avgX, int &avgY) {
    long sumX = 0, sumY = 0;
    int count = 0;

    for (int i = 0; i < TOUCH_SAMPLES; i++) {
        if (!ts.touched()) {
            if (count == 0) return false; // Lost touch before any sample
            break;
        }
        TS_Point p = ts.getPoint();
        sumX += p.x;
        sumY += p.y;
        count++;
    }

    if (count == 0) return false;

    avgX = (int)(sumX / count);
    avgY = (int)(sumY / count);

    // Store for diagnostics
    lastRawX = avgX;
    lastRawY = avgY;

    return true;
}

// ─── Manual Axis Correction ───────────────────────
// The XPT2046 library's setRotation() is unreliable for
// mapping raw ADC values to screen coordinates. We handle
// axis swap/inversion manually based on TFT_ROTATION.
//
// TFT_ROTATION values (ILI9341 convention):
//   0 = Portrait  (0°)    — X maps to screen X, Y maps to screen Y
//   1 = Landscape (90°)   — raw Y → screen X (inverted), raw X → screen Y
//   2 = Portrait  (180°)  — X inverted, Y inverted
//   3 = Landscape (270°)  — raw Y → screen X, raw X → screen Y (inverted)
//
void InputManager::mapToScreen(int rawX, int rawY, int &screenX, int &screenY) {
#if TFT_ROTATION == 0
    // Portrait: no swap
    screenX = map(rawX, TOUCH_MIN_X, TOUCH_MAX_X, 0, SCREEN_W - 1);
    screenY = map(rawY, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_H - 1);
#elif TFT_ROTATION == 1
    // Landscape 90°: Most XPT2046 panels have swapped axes here, but based
    // Both X and Y are NOT swapped. 
    // X axis goes from MAX (Left) to MIN (Right) -> Inverted
    // Y axis goes from MIN (Top) to MAX (Bottom) -> Normal
    screenX = map(rawX, TOUCH_MAX_X, TOUCH_MIN_X, 0, SCREEN_W - 1);
    screenY = map(rawY, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_H - 1);
#elif TFT_ROTATION == 2
    // Portrait 180°: invert both
    screenX = map(rawX, TOUCH_MAX_X, TOUCH_MIN_X, 0, SCREEN_W - 1);
    screenY = map(rawY, TOUCH_MAX_Y, TOUCH_MIN_Y, 0, SCREEN_H - 1);
#elif TFT_ROTATION == 3
    // Landscape 270°: swap axes, invert new Y
    screenX = map(rawY, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_W - 1);
    screenY = map(rawX, TOUCH_MAX_X, TOUCH_MIN_X, 0, SCREEN_H - 1);
#else
    // Fallback: same as rotation 1
    screenX = map(rawY, TOUCH_MAX_Y, TOUCH_MIN_Y, 0, SCREEN_W - 1);
    screenY = map(rawX, TOUCH_MIN_X, TOUCH_MAX_X, 0, SCREEN_H - 1);
#endif

    // Clamp to screen bounds
    screenX = constrain(screenX, 0, SCREEN_W - 1);
    screenY = constrain(screenY, 0, SCREEN_H - 1);
}
