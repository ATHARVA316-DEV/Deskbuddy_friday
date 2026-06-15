#ifndef WEATHER_APP_H
#define WEATHER_APP_H

#include "../friday_core/app_base.h"
#include "../display/display_manager.h"
#include "../display/ui_theme.h"
#include "../config.h"

// ═══════════════════════════════════════════════════
//  FRIDAY — Weather App
//  OpenWeatherMap API + Offline Fallback
// ═══════════════════════════════════════════════════

struct WeatherData {
    float temperature;     // Celsius
    int humidity;          // %
    float windSpeed;       // km/h
    char condition[24];    // "Clear", "Clouds", etc.
    char description[48];  // "scattered clouds"
    int conditionId;       // OWM condition code
    bool valid;            // Whether data is available
    unsigned long fetchTime; // When data was last fetched
};

class WeatherApp : public AppBase {
public:
    void onEnter() override;
    void onExit() override;
    void update() override;
    void onInput(InputEvent event) override;
    void draw() override;
    const char* getName() override { return "Weather"; }
    bool needsRedraw() override { return true; }

private:
    WeatherData data;
    bool fetching;
    unsigned long lastFetch;
    unsigned long lastDraw;
    bool forceRedraw;

    void fetchWeather();
    void drawWeatherIcon(int x, int y, int conditionId);
    void drawDashboard();
    const char* getTimeSinceSync();
};

#endif // WEATHER_APP_H
