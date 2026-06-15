#ifndef DESKBUDDY_APP_H
#define DESKBUDDY_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/friday_face.h"
#include "../display/ui_theme.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — DeskBuddy Mode
//  Idle AI companion with FRIDAY face + quotes
// ═══════════════════════════════════════════════════

class DeskBuddyApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "DeskBuddy"; }
    bool needsRedraw() override { return true; }

private:
    int currentQuote;
    unsigned long lastQuoteTime;
    unsigned long lastAnimTime;
    int animPhase;    // 0=idle, 1=love, 2=surprised
    bool faceDrawn;

    void drawOverlay();
    void nextQuote();

    static const int QUOTE_COUNT = 15;
    static const char* quotes[QUOTE_COUNT];
};

#endif // DESKBUDDY_APP_H
