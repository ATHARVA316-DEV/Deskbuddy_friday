#ifndef TOUCH_DEBUG_APP_H
#define TOUCH_DEBUG_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"

class TouchDebugApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "IRON MAN PLAYGROUND"; }

private:
    float ironManX;
    float ironManY;
    int targetX;
    int targetY;
    
    IronManState currentState;
    unsigned long stateTimer;
    
    bool isTouching;
    bool forceRedraw;

    // Movement history to detect fast drags
    int lastTargetX;
    int lastTargetY;
    unsigned long lastMoveTime;
};

#endif // TOUCH_DEBUG_APP_H
