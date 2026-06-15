#ifndef CALCULATOR_APP_H
#define CALCULATOR_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Calculator App (Touch Keyboard)
//  Full on-screen tappable keypad
// ═══════════════════════════════════════════════════

#define CALC_EXPR_MAX  32
#define CALC_ROWS      5
#define CALC_COLS      4
#define CALC_TOTAL_KEYS (CALC_ROWS * CALC_COLS)

class CalculatorApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "Calculator"; }

private:
    char expression[CALC_EXPR_MAX];
    int exprLen;
    double result;
    bool hasResult;
    bool forceRedraw;

    // 5x4 key grid labels
    static const char* keyLabels[CALC_TOTAL_KEYS];

    // Key grid layout
    int keyX(int col);
    int keyY(int row);
    int keyW();
    int keyH();

    int hitTestKey(int tx, int ty);
    void handleKeyPress(int keyIndex);
    void appendChar(const char* label);
    void evaluate();
    void clearExpr();
    void drawDisplay();
    void drawKeyboard();
    double parseAndEval(const char* expr);
};

#endif // CALCULATOR_APP_H
