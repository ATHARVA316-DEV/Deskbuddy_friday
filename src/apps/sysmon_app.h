#ifndef SYSMON_APP_H
#define SYSMON_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — System Monitor App
// ═══════════════════════════════════════════════════

class SysMonApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "System"; }
    bool needsRedraw() override { return true; }

private:
    unsigned long lastDraw;
    void drawDashboard();
    const char* formatUptime(unsigned long ms);
    void drawBarWithLabel(const char* label, const char* value, float pct,
                          int x, int y, int w, uint16_t color);
};

#endif // SYSMON_APP_H
