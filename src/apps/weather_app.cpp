#include "weather_app.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../utils/wifi_manager.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Weather App Implementation (Touch)
// ═══════════════════════════════════════════════════

// Touch button positions
#define REFRESH_BTN_X  120
#define REFRESH_BTN_Y  200
#define REFRESH_BTN_W   80
#define REFRESH_BTN_H   32

void WeatherApp::onEnter() {
    data.valid = false;
    data.temperature = 0;
    data.humidity = 0;
    data.windSpeed = 0;
    data.conditionId = 0;
    strcpy(data.condition, "---");
    strcpy(data.description, "Loading...");
    fetching = false;
    lastFetch = 0;
    lastDraw = 0;
    forceRedraw = true;

    DisplayManager::clearScreen();
    DisplayManager::drawHeader("WEATHER");

    // Immediate fetch
    if (WiFiManager::isConnected()) {
        fetchWeather();
    } else {
        strcpy(data.description, "Offline - No WiFi");
        forceRedraw = true;
    }
}

void WeatherApp::onExit() {
    // Nothing to clean up
}

void WeatherApp::onInput(InputEvent event) {
    if (event.type == InputType::TOUCH_TAP) {
        // Check refresh button
        if (InputManager::isInsideRect(event.touchX, event.touchY,
                REFRESH_BTN_X, REFRESH_BTN_Y, REFRESH_BTN_W, REFRESH_BTN_H)) {
            if (WiFiManager::isConnected()) {
                // Visual feedback: flash the button
                DisplayManager::flashButton(REFRESH_BTN_X, REFRESH_BTN_Y,
                        REFRESH_BTN_W, REFRESH_BTN_H, "Refresh", HUD_CYAN);
                fetchWeather();
                forceRedraw = true;
            }
        }
    }
}

void WeatherApp::update() {
    // Auto-refresh every WEATHER_INTERVAL
    unsigned long now = millis();
    if (data.valid && (now - lastFetch > WEATHER_INTERVAL)) {
        if (WiFiManager::isConnected()) {
            fetchWeather();
            forceRedraw = true;
        }
    }
}

void WeatherApp::draw() {
    unsigned long now = millis();
    if (!forceRedraw && (now - lastDraw < 1000)) return;
    lastDraw = now;
    forceRedraw = false;

    drawDashboard();
}

void WeatherApp::fetchWeather() {
    if (!WiFiManager::isConnected()) return;

    HTTPClient http;
    // Set strict 2-second timeout to prevent Watchdog Timer crashes on bad WiFi
    http.setTimeout(2000);

    char url[256];
    snprintf(url, sizeof(url),
             "%s?q=%s,%s&appid=%s&units=metric",
             OWM_ENDPOINT, WEATHER_CITY, WEATHER_COUNTRY, OWM_API_KEY);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getStream());

        if (!error) {
            data.temperature = doc["main"]["temp"].as<float>();
            data.humidity = doc["main"]["humidity"].as<int>();
            data.windSpeed = doc["wind"]["speed"].as<float>() * 3.6f; // m/s to km/h
            data.conditionId = doc["weather"][0]["id"].as<int>();

            const char* cond = doc["weather"][0]["main"].as<const char*>();
            if (cond) strncpy(data.condition, cond, sizeof(data.condition) - 1);

            const char* desc = doc["weather"][0]["description"].as<const char*>();
            if (desc) strncpy(data.description, desc, sizeof(data.description) - 1);

            data.valid = true;
            data.fetchTime = millis();

            Serial.printf("Weather: %.1f°C, %s\n", data.temperature, data.condition);
        } else {
            Serial.println("Weather JSON parse failed");
        }
    } else {
        Serial.printf("Weather HTTP error: %d\n", httpCode);
    }

    http.end();
    lastFetch = millis();
}

void WeatherApp::drawDashboard() {
    auto& tft = DisplayManager::tft;

    // Clear content area
    tft.fillRect(0, HEADER_HEIGHT, SCREEN_W, SCREEN_H - HEADER_HEIGHT, APP_BG);

    // Re-draw header
    DisplayManager::drawHeader("WEATHER");

    if (!data.valid) {
        // Offline state
        DisplayManager::drawHUDFrame(30, 60, 260, 120, HUD_ORANGE);
        DisplayManager::drawCenteredText("OFFLINE", 90, FONT_MEDIUM, HUD_ORANGE);
        DisplayManager::drawCenteredText(data.description, 120, FONT_SMALL, HUD_LIGHT_GRAY);
        DisplayManager::drawTouchButton(REFRESH_BTN_X, REFRESH_BTN_Y, REFRESH_BTN_W, REFRESH_BTN_H,
                                        "Retry", HUD_CYAN);
        return;
    }

    // Weather icon (large, centered)
    drawWeatherIcon(SCREEN_W / 2, 65, data.conditionId);

    // Temperature — hero display
    char tempBuf[12];
    snprintf(tempBuf, sizeof(tempBuf), "%.1fC", data.temperature);
    int tempY = 95;
    DisplayManager::drawCenteredText(tempBuf, tempY, FONT_LARGE, HUD_BRIGHT_CYAN);

    // Condition description
    DisplayManager::drawCenteredText(data.description, tempY + 30, FONT_SMALL, HUD_LIGHT_GRAY);

    // Divider
    tft.drawFastHLine(40, 145, 240, APP_BORDER);

    // Stats row
    int statY = 155;

    // Humidity
    tft.fillCircle(80, statY + 5, 3, HUD_CYAN);
    char humBuf[16];
    snprintf(humBuf, sizeof(humBuf), "%d%%", data.humidity);
    DisplayManager::drawText(humBuf, 90, statY, FONT_SMALL, HUD_WHITE);
    DisplayManager::drawText("Humidity", 60, statY + 14, FONT_TINY, HUD_GRAY);

    // Wind
    tft.fillCircle(210, statY + 5, 3, HUD_CYAN);
    char windBuf[16];
    snprintf(windBuf, sizeof(windBuf), "%.1f km/h", data.windSpeed);
    DisplayManager::drawText(windBuf, 220, statY, FONT_SMALL, HUD_WHITE);
    DisplayManager::drawText("Wind", 210, statY + 14, FONT_TINY, HUD_GRAY);

    // Refresh button
    DisplayManager::drawTouchButton(REFRESH_BTN_X, REFRESH_BTN_Y, REFRESH_BTN_W, REFRESH_BTN_H,
                                    "Refresh", HUD_CYAN);

    // City name
    char cityBuf[32];
    snprintf(cityBuf, sizeof(cityBuf), "%s, %s", WEATHER_CITY, WEATHER_COUNTRY);
    DisplayManager::drawCenteredText(cityBuf, 192, FONT_TINY, HUD_DIM_CYAN);

    // HUD frame
    DisplayManager::drawHUDFrame(5, HEADER_HEIGHT + 2, 310, SCREEN_H - HEADER_HEIGHT - 4, HUD_DIM_CYAN);
}

void WeatherApp::drawWeatherIcon(int x, int y, int conditionId) {
    auto& tft = DisplayManager::tft;

    // Draw weather icons using primitives based on OWM condition codes
    // 2xx = Thunderstorm, 3xx = Drizzle, 5xx = Rain, 6xx = Snow
    // 7xx = Atmosphere, 800 = Clear, 80x = Clouds

    if (conditionId == 800) {
        // Clear sky — Sun
        tft.fillCircle(x, y, 12, HUD_YELLOW);
        for (int i = 0; i < 8; i++) {
            float angle = i * 45.0f * DEG_TO_RAD;
            int x1 = x + (int)(16 * cos(angle));
            int y1 = y + (int)(16 * sin(angle));
            int x2 = x + (int)(22 * cos(angle));
            int y2 = y + (int)(22 * sin(angle));
            tft.drawLine(x1, y1, x2, y2, HUD_YELLOW);
        }
    } else if (conditionId >= 801 && conditionId <= 804) {
        // Clouds
        tft.fillCircle(x - 8, y, 10, HUD_LIGHT_GRAY);
        tft.fillCircle(x + 6, y - 4, 12, HUD_WHITE);
        tft.fillCircle(x + 14, y + 2, 8, HUD_LIGHT_GRAY);
        tft.fillRoundRect(x - 18, y, 36, 12, 4, HUD_LIGHT_GRAY);
    } else if (conditionId >= 500 && conditionId < 600) {
        // Rain
        tft.fillCircle(x - 6, y - 5, 8, HUD_LIGHT_GRAY);
        tft.fillCircle(x + 6, y - 8, 10, HUD_LIGHT_GRAY);
        tft.fillRoundRect(x - 14, y - 4, 28, 8, 3, HUD_LIGHT_GRAY);
        // Raindrops
        for (int i = 0; i < 4; i++) {
            int rx = x - 10 + i * 7;
            int ry = y + 8 + (i % 2) * 4;
            tft.drawLine(rx, ry, rx - 2, ry + 5, HUD_CYAN);
        }
    } else if (conditionId >= 200 && conditionId < 300) {
        // Thunderstorm
        tft.fillCircle(x - 6, y - 8, 8, HUD_GRAY);
        tft.fillCircle(x + 6, y - 10, 10, HUD_GRAY);
        tft.fillRoundRect(x - 14, y - 6, 28, 8, 3, HUD_GRAY);
        // Lightning bolt
        tft.drawLine(x, y + 4, x - 3, y + 10, HUD_YELLOW);
        tft.drawLine(x - 3, y + 10, x + 2, y + 10, HUD_YELLOW);
        tft.drawLine(x + 2, y + 10, x - 2, y + 18, HUD_YELLOW);
    } else if (conditionId >= 600 && conditionId < 700) {
        // Snow
        tft.fillCircle(x - 6, y - 5, 8, HUD_WHITE);
        tft.fillCircle(x + 6, y - 8, 10, HUD_WHITE);
        tft.fillRoundRect(x - 14, y - 4, 28, 8, 3, HUD_WHITE);
        // Snowflakes
        for (int i = 0; i < 3; i++) {
            int sx = x - 8 + i * 8;
            int sy = y + 8 + (i % 2) * 5;
            tft.drawPixel(sx, sy, HUD_WHITE);
            tft.drawPixel(sx - 1, sy - 1, HUD_WHITE);
            tft.drawPixel(sx + 1, sy - 1, HUD_WHITE);
            tft.drawPixel(sx - 1, sy + 1, HUD_WHITE);
            tft.drawPixel(sx + 1, sy + 1, HUD_WHITE);
        }
    } else {
        // Default: question mark
        DisplayManager::drawCenteredText("?", y - 6, FONT_MEDIUM, HUD_GRAY);
    }
}

const char* WeatherApp::getTimeSinceSync() {
    static char buf[16];
    unsigned long elapsed = (millis() - data.fetchTime) / 1000;

    if (elapsed < 60) {
        snprintf(buf, sizeof(buf), "Just now");
    } else if (elapsed < 3600) {
        snprintf(buf, sizeof(buf), "%lu min ago", elapsed / 60);
    } else {
        snprintf(buf, sizeof(buf), "%lu hr ago", elapsed / 3600);
    }
    return buf;
}
