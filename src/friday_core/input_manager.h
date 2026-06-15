#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "../config.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Input Manager (Touch Screen)
//  XPT2046 touch gestures: tap, long press, swipe,
//  drag, release — with multi-sample averaging
// ═══════════════════════════════════════════════════

enum class InputType {
    NONE,
    TOUCH_TAP,          // Quick tap — equivalent to "select"
    TOUCH_LONG_PRESS,   // Held touch — equivalent to "power menu"
    TOUCH_SWIPE_LEFT,   // Swipe left
    TOUCH_SWIPE_RIGHT,  // Swipe right — equivalent to "back"
    TOUCH_SWIPE_UP,     // Swipe up
    TOUCH_SWIPE_DOWN,   // Swipe down
    TOUCH_RELEASE,      // Finger lifted — for clearing visual feedback
    TOUCH_DRAG          // Finger moving while held — for scrolling
};

struct InputEvent {
    InputType type;
    int touchX;        // Screen X coordinate (0–319 in landscape)
    int touchY;        // Screen Y coordinate (0–239 in landscape)
    int deltaX;        // X movement since last poll (for drag/scroll)
    int deltaY;        // Y movement since last poll (for drag/scroll)
    float velocity;    // Swipe velocity (px/sec) for momentum
};

class InputManager {
public:
    static void init();
    static void poll();
    static InputEvent getEvent();

    // Check if currently touching (thread-safe)
    static bool isTouching();

    // Get last touch coordinates (valid during/after touch)
    static int getLastX();
    static int getLastY();

    // Utility: is a point inside a rectangle?
    static bool isInsideRect(int tx, int ty, int rx, int ry, int rw, int rh);

    static void getRawPoint(int &rawX, int &rawY);

    static XPT2046_Touchscreen ts;

    // Map raw XPT2046 values to screen coordinates
    static void mapToScreen(int rawX, int rawY, int &screenX, int &screenY);

private:

    // Touch state tracking (volatile since written by task, read by main thread)
    static volatile bool touchActive;
    static volatile int touchCurrentX;
    static volatile int touchCurrentY;
    static volatile int lastRawX;
    static volatile int lastRawY;

    // Task-local tracking (used only inside touchTask)
    static bool wasTouched;
    static int touchStartX;
    static int touchStartY;
    static int touchPrevX;
    static int touchPrevY;
    static unsigned long touchStartTime;
    static unsigned long lastEventTime;
    static bool longPressEmitted;
    static bool dragEmitted;

    // Event queue
    static QueueHandle_t inputQueue;
    static InputEvent currentEvent; // Current event pulled by main thread

    // FreeRTOS Task
    static void touchTask(void* pvParameters);

    // Multi-sample averaging: read N raw points and average
    static bool readAveragedPoint(int &avgX, int &avgY);

};

#endif // INPUT_MANAGER_H
