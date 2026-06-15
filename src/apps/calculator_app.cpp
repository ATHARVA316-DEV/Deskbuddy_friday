#include "calculator_app.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// ═══════════════════════════════════════════════════
//  FRIDAY — Calculator App Implementation (Touch)
//  Full on-screen tappable 5×4 keypad
// ═══════════════════════════════════════════════════

// 5 rows × 4 columns = 20 keys
// Layout:
//  [ 7 ] [ 8 ] [ 9 ] [ / ]
//  [ 4 ] [ 5 ] [ 6 ] [ * ]
//  [ 1 ] [ 2 ] [ 3 ] [ - ]
//  [ 0 ] [ . ] [(-)] [ + ]
//  [ C ] [sqrt] [x^2] [ = ]

const char* CalculatorApp::keyLabels[CALC_TOTAL_KEYS] = {
    "7", "8", "9", "/",
    "4", "5", "6", "*",
    "1", "2", "3", "-",
    "0", ".", "(-)", "+",
    "C", "sqrt", "x^2", "="
};

// Key grid layout constants
#define GRID_START_X  8
#define GRID_START_Y  85
#define GRID_PAD      4
#define KEY_WIDTH     72
#define KEY_HEIGHT    32

int CalculatorApp::keyX(int col) {
    return GRID_START_X + col * (KEY_WIDTH + GRID_PAD);
}

int CalculatorApp::keyY(int row) {
    return GRID_START_Y + row * (KEY_HEIGHT + GRID_PAD);
}

int CalculatorApp::keyW() { return KEY_WIDTH; }
int CalculatorApp::keyH() { return KEY_HEIGHT; }

void CalculatorApp::onEnter() {
    clearExpr();
    forceRedraw = true;

    DisplayManager::clearScreen();
    DisplayManager::drawHeader("CALCULATOR");
}

void CalculatorApp::onExit() {
    // Nothing
}

void CalculatorApp::update() {
    // Static app, no periodic updates
}

void CalculatorApp::onInput(InputEvent event) {
    if (event.type == InputType::TOUCH_TAP) {
        int keyIdx = hitTestKey(event.touchX, event.touchY);
        if (keyIdx >= 0) {
            // Visual feedback: flash the pressed key
            int r = keyIdx / CALC_COLS;
            int c = keyIdx % CALC_COLS;
            int kx = keyX(c);
            int ky = keyY(r);
            const char* label = keyLabels[keyIdx];
            DisplayManager::drawTouchButtonActive(kx, ky, KEY_WIDTH, KEY_HEIGHT, label, HUD_CYAN);
            delay(TOUCH_FEEDBACK_MS);

            handleKeyPress(keyIdx);
            forceRedraw = true;
        }
    }
}

int CalculatorApp::hitTestKey(int tx, int ty) {
    for (int r = 0; r < CALC_ROWS; r++) {
        for (int c = 0; c < CALC_COLS; c++) {
            int kx = keyX(c);
            int ky = keyY(r);
            // Added 5px padding on all sides to make buttons more forgiving to touch
            if (InputManager::isInsideRect(tx, ty, kx - 5, ky - 5, keyW() + 10, keyH() + 10)) {
                return r * CALC_COLS + c;
            }
        }
    }
    return -1;
}

void CalculatorApp::handleKeyPress(int keyIndex) {
    if (keyIndex < 0 || keyIndex >= CALC_TOTAL_KEYS) return;
    const char* label = keyLabels[keyIndex];
    appendChar(label);
}

void CalculatorApp::draw() {
    if (!forceRedraw) return;
    forceRedraw = false;

    drawDisplay();
    drawKeyboard();
}

void CalculatorApp::clearExpr() {
    memset(expression, 0, CALC_EXPR_MAX);
    exprLen = 0;
    result = 0;
    hasResult = false;
}

void CalculatorApp::appendChar(const char* label) {
    // Special actions
    if (strcmp(label, "=") == 0) {
        evaluate();
        return;
    }
    if (strcmp(label, "C") == 0) {
        clearExpr();
        return;
    }

    // Scientific unary operations
    if (strcmp(label, "sqrt") == 0) {
        double val = atof(expression);
        result = sqrt(fabs(val));
        hasResult = true;
        snprintf(expression, CALC_EXPR_MAX, "%.6g", result);
        exprLen = strlen(expression);
        return;
    }
    if (strcmp(label, "x^2") == 0) {
        double val = atof(expression);
        result = val * val;
        hasResult = true;
        snprintf(expression, CALC_EXPR_MAX, "%.6g", result);
        exprLen = strlen(expression);
        return;
    }

    // Negate
    if (strcmp(label, "(-)") == 0) {
        if (exprLen > 0 && expression[0] == '-') {
            memmove(expression, expression + 1, exprLen);
            exprLen--;
        } else if (exprLen < CALC_EXPR_MAX - 1) {
            memmove(expression + 1, expression, exprLen + 1);
            expression[0] = '-';
            exprLen++;
        }
        return;
    }

    // Append digit or operator
    if (exprLen < CALC_EXPR_MAX - 2) {
        // If we just showed a result and user types a digit, start fresh
        if (hasResult && label[0] >= '0' && label[0] <= '9') {
            clearExpr();
        }
        hasResult = false;

        strcat(expression, label);
        exprLen = strlen(expression);
    }
}

void CalculatorApp::evaluate() {
    result = parseAndEval(expression);
    hasResult = true;
}

double CalculatorApp::parseAndEval(const char* expr) {
    double numStack[20];
    char opStack[20];
    int numTop = 0;
    int opTop = 0;

    const char* p = expr;
    while (*p) {
        // Parse number
        if ((*p >= '0' && *p <= '9') || *p == '.' ||
            (*p == '-' && (p == expr || *(p-1) == '+' || *(p-1) == '-' || *(p-1) == '*' || *(p-1) == '/'))) {
            char* end;
            double current = strtod(p, &end);
            p = end;
            if (numTop < 20) numStack[numTop++] = current;
        } else {
            // Operator
            char op = *p;
            p++;
            
            // Apply higher/equal precedence operators from stack
            while (opTop > 0) {
                char prevOp = opStack[opTop - 1];
                if ((op == '*' || op == '/') && (prevOp == '+' || prevOp == '-')) {
                    break;
                }
                
                if (numTop >= 2) {
                    double b = numStack[--numTop];
                    double a = numStack[--numTop];
                    if (prevOp == '+') numStack[numTop++] = a + b;
                    else if (prevOp == '-') numStack[numTop++] = a - b;
                    else if (prevOp == '*') numStack[numTop++] = a * b;
                    else if (prevOp == '/') numStack[numTop++] = (b != 0) ? a / b : 0;
                }
                opTop--;
            }
            if (opTop < 20) opStack[opTop++] = op;
        }
    }

    // Apply remaining operators
    while (opTop > 0 && numTop >= 2) {
        char prevOp = opStack[--opTop];
        double b = numStack[--numTop];
        double a = numStack[--numTop];
        if (prevOp == '+') numStack[numTop++] = a + b;
        else if (prevOp == '-') numStack[numTop++] = a - b;
        else if (prevOp == '*') numStack[numTop++] = a * b;
        else if (prevOp == '/') numStack[numTop++] = (b != 0) ? a / b : 0;
    }

    return numTop > 0 ? numStack[0] : 0;
}

void CalculatorApp::drawDisplay() {
    auto& tft = DisplayManager::tft;

    // Expression display area
    tft.fillRect(10, 32, 300, 45, HUD_DARK_BLUE);
    DisplayManager::drawHUDFrame(10, 32, 300, 45, HUD_CYAN);

    // Expression text
    if (exprLen > 0) {
        DisplayManager::drawRightText(expression, 300, 36, FONT_MEDIUM, HUD_WHITE);
    } else {
        DisplayManager::drawRightText("0", 300, 36, FONT_MEDIUM, HUD_GRAY);
    }

    // Result line
    if (hasResult) {
        char resBuf[20];
        snprintf(resBuf, sizeof(resBuf), "= %.8g", result);
        DisplayManager::drawRightText(resBuf, 300, 56, FONT_SMALL, HUD_BRIGHT_CYAN);
    }
}

void CalculatorApp::drawKeyboard() {
    auto& tft = DisplayManager::tft;

    for (int r = 0; r < CALC_ROWS; r++) {
        for (int c = 0; c < CALC_COLS; c++) {
            int idx = r * CALC_COLS + c;
            int kx = keyX(c);
            int ky = keyY(r);
            const char* label = keyLabels[idx];

            // Color coding
            uint16_t borderCol = HUD_DIM_CYAN;
            uint16_t textCol = HUD_WHITE;

            // Operators column (rightmost)
            if (c == 3) {
                borderCol = HUD_CYAN;
                textCol = HUD_BRIGHT_CYAN;
            }
            // Bottom row special keys
            if (r == 4) {
                if (c == 0) {  // C
                    borderCol = HUD_ORANGE;
                    textCol = HUD_ORANGE;
                } else if (c == 3) {  // =
                    borderCol = HUD_GREEN;
                    textCol = HUD_GREEN;
                } else {
                    borderCol = HUD_CYAN;
                    textCol = HUD_CYAN;
                }
            }

            tft.fillRoundRect(kx, ky, KEY_WIDTH, KEY_HEIGHT, 4, HUD_DARK_BLUE);
            tft.drawRoundRect(kx, ky, KEY_WIDTH, KEY_HEIGHT, 4, borderCol);

            // Center label in key
            tft.setTextSize(FONT_SMALL);
            int16_t x1, y1;
            uint16_t tw, th;
            tft.getTextBounds(label, 0, 0, &x1, &y1, &tw, &th);
            tft.setCursor(kx + (KEY_WIDTH - tw) / 2, ky + (KEY_HEIGHT - th) / 2);
            tft.setTextColor(textCol);
            tft.print(label);
        }
    }
}
