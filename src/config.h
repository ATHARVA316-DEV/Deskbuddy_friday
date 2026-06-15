#ifndef CONFIG_H
#define CONFIG_H

// ═══════════════════════════════════════════════════
//  FRIDAY — Central Configuration
// ═══════════════════════════════════════════════════

// ─── User Settings ─────────────────────────────────
#define USER_NAME "User"
#define WEATHER_CITY "Mysuru"
#define WEATHER_COUNTRY "IN"
#define UTC_OFFSET_SEC 19800 // UTC+5:30 = 5*3600 + 30*60

// ─── WiFi Credentials ──────────────────────────────
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ─── OpenWeatherMap API ────────────────────────────
#define OWM_API_KEY "YOUR_API_KEY"
#define OWM_ENDPOINT "http://api.openweathermap.org/data/2.5/weather"
#define WEATHER_INTERVAL 600000 // 10 minutes in ms

// ─── TFT Display Pins ──────────────────────────────
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
// MOSI = GPIO23, SCK = GPIO18, MISO = GPIO19 (hardware SPI)

// ─── Touch Pins (XPT2046 sharing SPI bus) ──────────
#define TOUCH_CS 5   // Touch chip select (separate from TFT)
#define TOUCH_IRQ 35 // Touch interrupt (input only pin)

// ─── Touch Tuning ──────────────────────────────────
#define TOUCH_DEBOUNCE_MS   30       // Touch debounce window (snappier taps)
#define TOUCH_LONG_PRESS_MS 600      // Long press threshold (ms)
#define TOUCH_SWIPE_THRESH  30       // Min px delta for swipe detection
#define TOUCH_MIN_X         200      // Raw touch calibration min X
#define TOUCH_MAX_X         3800     // Raw touch calibration max X
#define TOUCH_MIN_Y         200      // Raw touch calibration min Y
#define TOUCH_MAX_Y         3800     // Raw touch calibration max Y
#define TOUCH_SAMPLES       3        // Raw readings to average (jitter reduction)
#define TOUCH_FEEDBACK_MS   80       // Visual highlight duration on button press
#define TOUCH_DRAG_THRESH   8        // Min px delta to emit TOUCH_DRAG event

// ─── Back Button (global, consistent) ─────────────
#define BACK_BTN_X          0
#define BACK_BTN_Y          0
#define BACK_BTN_W          100      // Massive touch target width
#define BACK_BTN_H          45       // Massive touch target height

// ─── Timing ────────────────────────────────────────
#define NTP_SYNC_INTERVAL 900000 // 15 minutes in ms
#define WIFI_TIMEOUT 10000       // 10 seconds connection timeout
#define SLEEP_TIMEOUT 300000     // 5 minutes idle → auto sleep
#define QUOTE_INTERVAL 15000     // DeskBuddy quote rotation (15 sec)

// ─── Display ───────────────────────────────────────
#define SCREEN_W 320
#define SCREEN_H 240
#define TFT_SPI_FREQ 40000000 // 40 MHz SPI clock
#define TFT_ROTATION 1        // Landscape

// ─── Menu ──────────────────────────────────────────
#define MENU_ITEMS 7       // Number of apps in radial menu
#define MENU_RADIUS 78     // Orbit radius for app icons
#define MENU_ICON_R 18     // Normal icon radius
#define MENU_ICON_R_SEL 24 // Selected icon radius
#define MENU_CENTER_X 160  // Menu center X
#define MENU_CENTER_Y 120  // Menu center Y
#define MENU_PULL_IN 12    // Selected icon pull-toward-center px

#endif // CONFIG_H
