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

### Additional Inputs
- **Tactile Button #1**: [Insert Pin here]
- **Tactile Button #2**: [Insert Pin here]
- **Potentiometer**: [Insert Analog Pin here]

*(Leave space below for connection schematic screenshot)*

---
`[📸 Insert Schematic / Breadboard Screenshot Here]`
---

## 🕹️ How to Use

FRIDAY is designed to be operated entirely without a keyboard, using only two buttons and one potentiometer.

### Control Scheme

**Potentiometer (Rotary Selector):**
- **Rotate Clockwise:** Navigate menu forward / down.
- **Rotate Counterclockwise:** Navigate menu backward / up.

**Button 1 (System / Power):**
- **Short Press:** Sleep / Wake FRIDAY.
- **Long Press:** Open Power Menu.

**Button 2 (Action / Select):**
- **Short Press:** Select / Confirm / Enter Application.

### Main Radial Menu Navigation

The main menu features an "Arc Reactor" style circular layout. As you turn the potentiometer, the outer icons will smoothly rotate around the center "FRIDAY Core". 
To select an app, rotate until the desired app is highlighted (glowing and enlarged), then press **Button 2**.

*(Leave space below for UI screenshots)*

---
`[📸 Insert Radial Menu Screenshot Here]`
---

---
`[📸 Insert App Interface Screenshot Here]`
---
