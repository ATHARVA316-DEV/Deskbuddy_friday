#include "toolbox_app.h"
#include <math.h>

// ═══════════════════════════════════════════════════
//  FRIDAY — Engineering Toolbox Implementation (Touch)
// ═══════════════════════════════════════════════════

const char* ToolboxApp::toolNames[5] = {
    "Ohm's Law",
    "Voltage Divider",
    "RC Time Constant",
    "LED Current",
    "Frequency"
};

// Touch layout for tool selector
#define TOOL_LIST_X     15
#define TOOL_LIST_W    290
#define TOOL_LIST_H     32
#define TOOL_LIST_Y0    40
#define TOOL_LIST_GAP   34

// Param +/- buttons in tool mode
#define PARAM_MINUS_X   190
#define PARAM_PLUS_X    255
#define PARAM_BTN_W      50
#define PARAM_BTN_H      30

// Param +/- buttons in tool mode
#define PARAM_MINUS_X   190
#define PARAM_PLUS_X    255
#define PARAM_BTN_W      50
#define PARAM_BTN_H      30

void ToolboxApp::onEnter() {
    currentTool = ToolType::OHMS_LAW;
    inTool = false;
    paramIndex = 0;
    memset(params, 0, sizeof(params));
    result = 0;
    forceRedraw = true;

    DisplayManager::clearScreen();
    DisplayManager::drawHeader("ENGINEERING TOOLBOX");
}

bool ToolboxApp::onBack() {
    if (inTool) {
        // We are inside a tool. Consume the back press to return to the tool list.
        inTool = false;
        forceRedraw = true;
        DisplayManager::clearScreen();
        DisplayManager::drawHeader("ENGINEERING TOOLBOX");
        return true;
    }
    // We are on the main toolbox list. Let StateMachine handle returning to MENU.
    return false;
}

void ToolboxApp::onExit() {
    // Nothing
}

void ToolboxApp::update() {
    // Static
}

void ToolboxApp::onInput(InputEvent event) {
    if (!inTool) {
        // Tool selector mode
        if (event.type == InputType::TOUCH_TAP) {
            // Check which tool was tapped
            for (int i = 0; i < 5; i++) {
                int y = TOOL_LIST_Y0 + i * TOOL_LIST_GAP;
                if (InputManager::isInsideRect(event.touchX, event.touchY,
                        TOOL_LIST_X, y, TOOL_LIST_W, TOOL_LIST_H)) {
                    // Visual feedback: flash the selected tool item
                    DisplayManager::drawTouchButtonActive(TOOL_LIST_X, y, TOOL_LIST_W, TOOL_LIST_H,
                                                           toolNames[i], HUD_CYAN);
                    delay(TOUCH_FEEDBACK_MS);

                    currentTool = (ToolType)i;
                    inTool = true;
                    paramIndex = 0;
                    
                    // Set realistic defaults so formulas generate immediately
                    switch (currentTool) {
                        case ToolType::OHMS_LAW: 
                            params[0] = 5.0f; params[1] = 0.02f; break; // 5V, 20mA
                        case ToolType::VOLTAGE_DIVIDER: 
                            params[0] = 5.0f; params[1] = 1000.0f; params[2] = 1000.0f; break; // 5V, 1k, 1k
                        case ToolType::RC_CONSTANT: 
                            params[0] = 1000.0f; params[1] = 0.0001f; break; // 1k, 100uF
                        case ToolType::LED_CURRENT: 
                            params[0] = 5.0f; params[1] = 2.0f; params[2] = 0.02f; break; // 5V, 2V drop, 20mA
                        case ToolType::FREQUENCY: 
                            params[0] = 0.001f; break; // 1ms = 1kHz
                        default: 
                            memset(params, 0, sizeof(params)); break;
                    }
                    calculate();

                    forceRedraw = true;
                    DisplayManager::clearScreen();
                    DisplayManager::drawHeader(toolNames[i]);
                    return;
                }
            }
        }
    } else {
        // Inside a tool
        if (event.type == InputType::TOUCH_TAP) {
            int tx = event.touchX;
            int ty = event.touchY;

            // Back button is now handled entirely by onBack() called via StateMachine.
            
            // Check param rows — tap row to select it
            int maxParams = 3;
            switch (currentTool) {
                case ToolType::OHMS_LAW: maxParams = 2; break;
                case ToolType::VOLTAGE_DIVIDER: maxParams = 3; break;
                case ToolType::RC_CONSTANT: maxParams = 2; break;
                case ToolType::LED_CURRENT: maxParams = 3; break;
                case ToolType::FREQUENCY: maxParams = 1; break;
                default: break;
            }

            for (int i = 0; i < maxParams; i++) {
                int py = 58 + i * 28;

                // Tap on - button
                if (InputManager::isInsideRect(tx, ty, PARAM_MINUS_X, py, PARAM_BTN_W, PARAM_BTN_H)) {
                    paramIndex = i;
                    params[i] = max(0.0f, params[i] - getParamStep());
                    calculate();
                    forceRedraw = true;
                    // Visual feedback on - button
                    DisplayManager::drawTouchButtonActive(PARAM_MINUS_X, py, PARAM_BTN_W, PARAM_BTN_H, "-", HUD_CYAN);
                    delay(TOUCH_FEEDBACK_MS / 2);
                    return;
                }
                // Tap on + button
                if (InputManager::isInsideRect(tx, ty, PARAM_PLUS_X, py, PARAM_BTN_W, PARAM_BTN_H)) {
                    paramIndex = i;
                    params[i] = min(getParamMax(), params[i] + getParamStep());
                    calculate();
                    forceRedraw = true;
                    // Visual feedback on + button
                    DisplayManager::drawTouchButtonActive(PARAM_PLUS_X, py, PARAM_BTN_W, PARAM_BTN_H, "+", HUD_CYAN);
                    delay(TOUCH_FEEDBACK_MS / 2);
                    return;
                }

                // Tap on row to select param
                if (InputManager::isInsideRect(tx, ty, 10, py, 185, PARAM_BTN_H)) {
                    paramIndex = i;
                    forceRedraw = true;
                    return;
                }
            }
        }
    }
}

float ToolboxApp::getParamStep() {
    switch (currentTool) {
        case ToolType::OHMS_LAW:
            return (paramIndex == 0) ? 0.5f : 0.01f;
        case ToolType::VOLTAGE_DIVIDER:
            return (paramIndex == 0) ? 0.5f : 100.0f;
        case ToolType::RC_CONSTANT:
            return (paramIndex == 0) ? 100.0f : 0.00001f;
        case ToolType::LED_CURRENT:
            return (paramIndex == 0) ? 0.5f : (paramIndex == 1 ? 0.1f : 0.001f);
        case ToolType::FREQUENCY:
            return 0.01f;
        default: return 1.0f;
    }
}

float ToolboxApp::getParamMax() {
    switch (currentTool) {
        case ToolType::OHMS_LAW:
            return (paramIndex == 0) ? 50.0f : 5.0f;
        case ToolType::VOLTAGE_DIVIDER:
            return (paramIndex == 0) ? 50.0f : 100000.0f;
        case ToolType::RC_CONSTANT:
            return (paramIndex == 0) ? 100000.0f : 0.001f;
        case ToolType::LED_CURRENT:
            return (paramIndex == 0) ? 24.0f : (paramIndex == 1 ? 5.0f : 0.05f);
        case ToolType::FREQUENCY:
            return 1.0f;
        default: return 1000.0f;
    }
}

void ToolboxApp::draw() {
    if (!forceRedraw) return;
    forceRedraw = false;

    if (!inTool) {
        drawToolSelector();
    } else {
        switch (currentTool) {
            case ToolType::OHMS_LAW:        drawOhmsLaw(); break;
            case ToolType::VOLTAGE_DIVIDER:  drawVoltageDivider(); break;
            case ToolType::RC_CONSTANT:      drawRCConstant(); break;
            case ToolType::LED_CURRENT:      drawLEDCurrent(); break;
            case ToolType::FREQUENCY:        drawFrequency(); break;
            default: break;
        }
    }
}

void ToolboxApp::calculate() {
    switch (currentTool) {
        case ToolType::OHMS_LAW:
            // V = I * R  (params[0]=V, params[1]=I → R)
            if (params[1] > 0) result = params[0] / params[1];
            break;

        case ToolType::VOLTAGE_DIVIDER:
            // Vout = Vin * R2 / (R1 + R2)
            if ((params[1] + params[2]) > 0)
                result = params[0] * params[2] / (params[1] + params[2]);
            break;

        case ToolType::RC_CONSTANT:
            // τ = R * C
            result = params[0] * params[1];
            break;

        case ToolType::LED_CURRENT:
            // R = (Vs - Vf) / If
            if (params[2] > 0)
                result = (params[0] - params[1]) / params[2];
            break;

        case ToolType::FREQUENCY:
            // f = 1 / T
            if (params[0] > 0) result = 1.0f / params[0];
            break;

        default: break;
    }
}

void ToolboxApp::drawToolSelector() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("ENGINEERING TOOLBOX");

    for (int i = 0; i < 5; i++) {
        int y = TOOL_LIST_Y0 + i * TOOL_LIST_GAP;

        tft.fillRoundRect(TOOL_LIST_X, y, TOOL_LIST_W, TOOL_LIST_H, 6, HUD_DARK_BLUE);
        tft.drawRoundRect(TOOL_LIST_X, y, TOOL_LIST_W, TOOL_LIST_H, 6, HUD_DIM_CYAN);
        // Arrow indicator
        tft.fillTriangle(TOOL_LIST_X + 7, y + 8, TOOL_LIST_X + 7, y + 20,
                         TOOL_LIST_X + 15, y + 14, HUD_CYAN);
        DisplayManager::drawText(toolNames[i], TOOL_LIST_X + 21, y + 6, FONT_MEDIUM, HUD_LIGHT_GRAY);
    }

    DisplayManager::drawCenteredText("Tap a tool to open", SCREEN_H - 14, FONT_TINY, HUD_GRAY);
    DisplayManager::drawHUDFrame(5, HEADER_HEIGHT + 2, 310, SCREEN_H - HEADER_HEIGHT - 4, HUD_DIM_CYAN);
}

void ToolboxApp::drawParamEditor(const char* label, float value, int index, int y, const char* unit) {
    auto& tft = DisplayManager::tft;
    bool active = (paramIndex == index);

    uint16_t col = active ? HUD_BRIGHT_CYAN : HUD_GRAY;
    uint16_t valCol = active ? HUD_WHITE : HUD_LIGHT_GRAY;

    if (active) {
        tft.fillRoundRect(10, y - 2, 185, 22, 4, HUD_DARK_BLUE);
        tft.drawRoundRect(10, y - 2, 185, 22, 4, HUD_CYAN);
    }

    DisplayManager::drawText(label, 16, y + 2, FONT_SMALL, col);

    char buf[20];
    snprintf(buf, sizeof(buf), "%.4g %s", value, unit);
    DisplayManager::drawText(buf, 100, y + 2, FONT_SMALL, valCol);

    if (active) {
        tft.fillTriangle(6, y + 4, 6, y + 14, 10, y + 9, HUD_CYAN);
    }

    // +/- buttons
    DisplayManager::drawTouchButton(PARAM_MINUS_X, y, PARAM_BTN_W, PARAM_BTN_H, "-",
                                    active ? HUD_CYAN : HUD_DIM_CYAN);
    DisplayManager::drawTouchButton(PARAM_PLUS_X, y, PARAM_BTN_W, PARAM_BTN_H, "+",
                                    active ? HUD_CYAN : HUD_DIM_CYAN);
}

void ToolboxApp::drawResultLine(const char* label, float value, int y, const char* unit) {
    auto& tft = DisplayManager::tft;
    tft.drawFastHLine(20, y - 6, 280, HUD_CYAN);

    DisplayManager::drawText(label, 16, y, FONT_SMALL, HUD_CYAN);

    char buf[24];
    snprintf(buf, sizeof(buf), "%.6g %s", value, unit);
    DisplayManager::drawRightText(buf, 300, y, FONT_MEDIUM, HUD_BRIGHT_CYAN);
}

// ─── Individual Tool Screens ───────────────────────

void ToolboxApp::drawOhmsLaw() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("Ohm's Law");

    // Formula
    DisplayManager::drawCenteredText("V = I x R", 38, FONT_SMALL, HUD_DIM_CYAN);

    drawParamEditor("Voltage (V)", params[0], 0, 60, "V");
    drawParamEditor("Current (I)", params[1], 1, 90, "A");

    // Result: R = V / I
    drawResultLine("Resistance =", result, 130, "Ohm");

    // Removed hardcoded back button draw. Global header handles it natively.
}

void ToolboxApp::drawVoltageDivider() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("Voltage Divider");

    DisplayManager::drawCenteredText("Vout = Vin * R2/(R1+R2)", 38, FONT_TINY, HUD_DIM_CYAN);

    drawParamEditor("Vin", params[0], 0, 58, "V");
    drawParamEditor("R1", params[1], 1, 86, "Ohm");
    drawParamEditor("R2", params[2], 2, 114, "Ohm");

    drawResultLine("Vout =", result, 150, "V");

    // Removed hardcoded back button draw. Global header handles it natively.
}

void ToolboxApp::drawRCConstant() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("RC Time Constant");

    DisplayManager::drawCenteredText("tau = R x C", 38, FONT_SMALL, HUD_DIM_CYAN);

    drawParamEditor("Resistance", params[0], 0, 65, "Ohm");
    drawParamEditor("Capacitance", params[1], 1, 95, "F");

    drawResultLine("Tau =", result, 135, "sec");

    // Additional info
    char t5Buf[24];
    snprintf(t5Buf, sizeof(t5Buf), "5tau = %.6g sec", result * 5);
    DisplayManager::drawCenteredText(t5Buf, 170, FONT_TINY, HUD_LIGHT_GRAY);
    DisplayManager::drawCenteredText("(99.3% charged)", 184, FONT_TINY, HUD_GRAY);

    // Removed hardcoded back button draw. Global header handles it natively.
}

void ToolboxApp::drawLEDCurrent() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("LED Resistor");

    DisplayManager::drawCenteredText("R = (Vs - Vf) / If", 38, FONT_SMALL, HUD_DIM_CYAN);

    drawParamEditor("Source (Vs)", params[0], 0, 58, "V");
    drawParamEditor("LED Fwd (Vf)", params[1], 1, 86, "V");
    drawParamEditor("LED Current", params[2], 2, 114, "A");

    drawResultLine("Resistor =", result, 150, "Ohm");

    // Power dissipation
    if (params[2] > 0 && result > 0) {
        float power = params[2] * params[2] * result;
        char pBuf[24];
        snprintf(pBuf, sizeof(pBuf), "Power: %.4g W", power);
        DisplayManager::drawCenteredText(pBuf, 180, FONT_TINY, HUD_LIGHT_GRAY);
    }

    // Removed hardcoded back button draw. Global header handles it natively.
}

void ToolboxApp::drawFrequency() {
    auto& tft = DisplayManager::tft;
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);
    DisplayManager::drawHeader("Frequency");

    DisplayManager::drawCenteredText("f = 1 / T", 42, FONT_SMALL, HUD_DIM_CYAN);

    drawParamEditor("Period (T)", params[0], 0, 75, "sec");

    drawResultLine("Frequency =", result, 120, "Hz");

    // Also show angular frequency
    if (result > 0) {
        float omega = result * 2.0f * PI;
        char oBuf[24];
        snprintf(oBuf, sizeof(oBuf), "Omega: %.4g rad/s", omega);
        DisplayManager::drawCenteredText(oBuf, 160, FONT_TINY, HUD_LIGHT_GRAY);
    }

    // Removed hardcoded back button draw. Global header handles it natively.
}
