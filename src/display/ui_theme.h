#ifndef UI_THEME_H
#define UI_THEME_H

#include <Arduino.h>

// ═══════════════════════════════════════════════════
//  FRIDAY — Iron Man HUD Theme
//  All colors in RGB565 format
// ═══════════════════════════════════════════════════

// ─── Primary Palette ───────────────────────────────
#define HUD_BLACK          0x0000   // True black
#define HUD_DARK           0x0841   // Near-black background
#define HUD_DARK_BLUE      0x0011   // Deep navy
#define HUD_BLUE           0x1C1A   // Secondary blue
#define HUD_CYAN           0x2E1F   // Primary accent — arc reactor
#define HUD_BRIGHT_CYAN    0x07FF   // Full cyan — glow effects
#define HUD_DIM_CYAN       0x0B6D   // Muted cyan — unselected
#define HUD_WHITE          0xFFFF   // Text highlight
#define HUD_LIGHT_GRAY     0xC618   // Subtle text
#define HUD_GRAY           0x7BEF   // Disabled / secondary text

// ─── Accent Colors ─────────────────────────────────
#define HUD_ORANGE         0xFDA0   // Warning / attention
#define HUD_RED            0xF800   // Error / critical
#define HUD_GREEN          0x07E0   // Success / OK
#define HUD_YELLOW         0xFFE0   // Sparkle / highlight
#define HUD_PINK           0xFB2C   // Blush / heart
#define HUD_TOUCH_HI       0x5F1F   // Bright cyan flash for touch feedback

// ─── Character Colors ──────────────────────────────
#define CHAR_HELMET        0xE71C   // Helmet rim gold
#define CHAR_HELMET_HI     0xFFFF   // Helmet highlight
#define CHAR_VISOR         0x10A2   // Visor background
#define CHAR_EYE_GLOW      0x2E1F   // Eye cyan glow
#define CHAR_EYE_DIM       0x1C1A   // Eye halo
#define CHAR_EYE_HI        0xFFFF   // Eye specular
#define CHAR_BODY          0xE71C   // Body gold
#define CHAR_BODY_SHADOW   0xB596   // Body shadow
#define CHAR_HEART         0xF800   // Heart red
#define CHAR_HEART_HI      0xFBCF   // Heart highlight

// ─── Menu Theme ────────────────────────────────────
#define MENU_BG            HUD_DARK
#define MENU_RING_COL      0x10A2   // Orbit ring color
#define MENU_ICON_NORMAL   HUD_DIM_CYAN
#define MENU_ICON_SELECTED HUD_BRIGHT_CYAN
#define MENU_GLOW_COL      0x0B6D   // Selection halo
#define MENU_TEXT_COL       HUD_WHITE
#define MENU_LABEL_COL     HUD_CYAN
#define MENU_CORE_COL      HUD_CYAN
#define MENU_CORE_RING     0x10A2   // Inner core ring
#define MENU_TICK_COL      0x0B6D   // HUD tick marks

// ─── Semantic Theme Colors ───────────────────────
// HUD Standard
#define APP_BG        HUD_DARK
#define APP_BAR_BG    HUD_DARK_BLUE

// Iron Man Themed Transition Colors
#define IRON_RED      0xB800      // Metallic Red (~ #B30000)
#define IRON_GOLD     0xFE60      // Bright Gold/Yellow (~ #FFCC00)
#define APP_TEXT_PRIMARY    HUD_WHITE
#define APP_TEXT_SECONDARY  HUD_LIGHT_GRAY
#define APP_TEXT_DIM        HUD_GRAY
#define APP_ACCENT          HUD_CYAN
#define APP_BORDER          0x10A2   // Subtle border lines
#define APP_BAR_FILL       HUD_CYAN
// #define APP_BAR_BG         0x10A2   // Progress bar background (Removed to prevent redefinition)

// ─── Typography (Adafruit GFX text sizes) ──────────
#define FONT_TINY          1        // 6x8  — labels, small info
#define FONT_SMALL         1        // 6x8  — body text
#define FONT_MEDIUM        2        // 12x16 — subheadings
#define FONT_LARGE         3        // 18x24 — headings
#define FONT_XLARGE        4        // 24x32 — hero text
#define FONT_HUGE          5        // 30x40 — time display

// ─── Layout Constants ──────────────────────────────
#define HEADER_HEIGHT      32       // Top header bar height (enlarged for touch)
#define HEADER_Y           0        // Header Y position
#define CONTENT_Y          36       // Content area start
#define FOOTER_Y           220      // Footer area
#define MARGIN_X           8        // Horizontal margin
#define MARGIN_Y           4        // Vertical margin
#define CORNER_R           8        // Rounded corner radius (touch-friendly)

#endif // UI_THEME_H
