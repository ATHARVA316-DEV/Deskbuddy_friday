# FRIDAY - Connection & Usage Guide

This document outlines the hardware connections required to build FRIDAY and explains how to navigate the user interface.

## 🔌 Hardware Connections

### ESP32 to 2.4-inch SPI TFT Display (ILI9341)
| TFT Pin | ESP32 Pin |
|---------|-----------|
| VCC     | 3.3V      |
| GND     | GND       |
| CS      | GPIO15    |
| RESET   | GPIO4     |
| DC/RS   | GPIO2     |
| SDI (MOSI)| GPIO23  |
| SCK     | GPIO18    |
| LED     | 3.3V      |
| SDO (MISO)| GPIO19  |

### Touch Controls (XPT2046 Controller)
The display module includes a built-in XPT2046 touch controller that shares the SPI bus with the TFT.

| Touch Pin | ESP32 Pin | Description |
|-----------|-----------|-------------|
| T_CLK     | GPIO18    | Shares SCK with display |
| T_DIN     | GPIO23    | Shares MOSI with display |
| T_DO      | GPIO19    | Shares MISO with display |
| T_CS      | GPIO5     | Separate CS, activates touch IC independently |
| T_IRQ     | GPIO35    | Touch interrupt (optional, input only) |

*(Leave space below for connection schematic screenshot)*

---
`[📸 Insert Schematic / Breadboard Screenshot Here]`
---

## 🕹️ How to Use

FRIDAY is designed to be operated entirely via its 2.4-inch Touch Screen, providing a sleek, buttonless experience.

### Control Scheme

**Touch Screen Interactions:**
- **Tap:** Select an app, push a button, or confirm an action.
- **Swipe / Drag:** Navigate through menus, scroll text, or spin the radial menu.
- **Long Press:** Used for special actions like returning to the home screen or opening system menus.

### Main Radial Menu Navigation

The main menu features an "Arc Reactor" style circular layout.
- **To navigate:** Swipe across the screen to smoothly rotate the outer app icons around the "FRIDAY Core".
- **To select:** Tap on an app icon when you want to launch it. The app icon will glow and enlarge when tapped.

*(Leave space below for UI screenshots)*

---
`[📸 Insert Radial Menu Screenshot Here]`
---

---
`[📸 Insert App Interface Screenshot Here]`
---
