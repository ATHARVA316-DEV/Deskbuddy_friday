#ifndef TOOLBOX_APP_H
#define TOOLBOX_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Engineering Toolbox (Touch)
//  Ohm's Law, Voltage Divider, RC, LED, Frequency
// ═══════════════════════════════════════════════════

enum class ToolType {
    OHMS_LAW = 0,
    VOLTAGE_DIVIDER,
    RC_CONSTANT,
    LED_CURRENT,
    FREQUENCY,
    TOOL_COUNT
};

class ToolboxApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "ENGINEERING TOOLBOX"; }
    bool onBack() override;

private:
    ToolType currentTool;
    bool inTool;           // false = tool selector, true = inside a tool
    int paramIndex;        // Which parameter is being edited
    float params[4];       // Up to 4 input parameters
    float result;
    bool forceRedraw;

    // Param adjustment step
    float getParamStep();
    float getParamMax();

    void drawToolSelector();
    void drawOhmsLaw();
    void drawVoltageDivider();
    void drawRCConstant();
    void drawLEDCurrent();
    void drawFrequency();
    void calculate();
    void drawParamEditor(const char* label, float value, int index, int y, const char* unit);
    void drawResultLine(const char* label, float value, int y, const char* unit);

    static const char* toolNames[5];
};

#endif // TOOLBOX_APP_H
