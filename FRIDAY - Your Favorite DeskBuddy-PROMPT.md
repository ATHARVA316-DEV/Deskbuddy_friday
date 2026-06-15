**FRIDAY \- Your Favorite DeskBuddy**.

---

# **MASTER PROMPT**

You are a senior embedded systems engineer, UI/UX designer, firmware architect, and product designer.

I am building an ESP32-based smart desktop assistant called **FRIDAY (Your Favorite DeskBuddy)**.

The goal is to create a futuristic, highly polished, Iron-Man-inspired desk companion that combines productivity, utilities, engineering tools, weather information, time management, and entertainment into a single handheld device.

---

# **HARDWARE**

Microcontroller:

* ESP32-WROOM-32 NodeMCU (38 Pin)

Display:

* 2.4-inch SPI TFT Display  
* Driver: ILI9341  
* Resolution: 240x320  
* Touch capability available but not required for Version 1.0

Current Connections:

TFT → ESP32

* VCC → 3.3V  
* GND → GND  
* CS → GPIO15  
* RESET → GPIO4  
* DC/RS → GPIO2  
* SDI (MOSI) → GPIO23  
* SCK → GPIO18  
* LED → GPIO3.3V  
* SDO (MISO) → GPIO19

Additional Inputs:

* Tactile Button \#1  
* Tactile Button \#2  
* Potentiometer

Control Scheme:

Button 1:

* Short Press → Sleep/Wake FRIDAY  
* Long Press → Power Menu

Button 2:

* Select  
* Confirm  
* Enter Application

Potentiometer:

* Rotate Clockwise → Navigate Menu  
* Rotate Counterclockwise → Navigate Menu  
* Acts like a rotary selector

No keyboard available.

All navigation must be possible using only:

* 2 Buttons  
* 1 Potentiometer

---

# **PROJECT NAME**

FRIDAY

Tagline:

"Your Favorite DeskBuddy"

---

# **DESIGN PHILOSOPHY**

The device should feel like:

* Tony Stark's FRIDAY  
* Iron Man HUD  
* Futuristic AI Assistant  
* Premium Consumer Product  
* Smart Desktop Companion

Avoid:

* Basic Arduino-style menus  
* Text-heavy interfaces  
* Ugly list menus

Focus on:

* Smooth animations  
* Circular UI  
* Futuristic themes  
* Modern visual design  
* Intuitive navigation

---

# **MAIN MENU DESIGN**

I DO NOT WANT A LIST MENU.

I WANT A CIRCULAR RADIAL MENU.

The menu should resemble:

* Arc Reactor  
* Smartwatch Launcher  
* Iron Man HUD

Center:

FRIDAY Core

Surrounding the center:

Apps arranged around a circle.

Example:

       Weather

  Files         Games

Clock FRIDAY Notes

 Tools         Timer

     Calculator

The currently selected app should:

* Enlarge  
* Glow  
* Highlight  
* Move slightly toward the center

The menu should rotate smoothly when the potentiometer moves.

Use trigonometry:

x \= centerX \+ radius × cos(angle)

y \= centerY \+ radius × sin(angle)

Provide:

* UI mockups  
* Wireframes  
* Animation ideas  
* Navigation logic  
* State machine architecture

---

# **VERSION 1.0 FEATURES**

Design the complete firmware architecture for:

1. Clock

Features:

* Digital Clock  
* Analog Clock  
* Stopwatch  
* Countdown Timer

---

2. Weather Station

Using WiFi APIs.

Display:

* Current Temperature  
* Humidity  
* Weather Condition  
* Wind Speed

Include:

* Offline fallback behavior

---

3. Calculator

Basic:

* Addition  
* Subtraction  
* Multiplication  
* Division

Advanced:

* Square Root  
* Powers  
* Logarithm  
* Trigonometry

Design an input system that works using only:

* Potentiometer  
* 2 Buttons

---

4. System Monitor

Display:

* Free RAM  
* CPU Frequency  
* WiFi Status  
* Uptime

---

5. Engineering Toolbox

Include:

* Ohm's Law Calculator  
* Voltage Divider Calculator  
* RC Time Constant Calculator  
* LED Current Calculator  
* Frequency Calculator

Provide formulas and implementation logic.

---

6. Pomodoro Timer

Features:

* 25/5 Mode  
* Custom Mode  
* Session Counter

---

7. DeskBuddy Mode

Idle mode where FRIDAY behaves like an AI companion.

Display:

* Time  
* Weather  
* Motivational Quotes  
* Funny Messages  
* Engineering Jokes

Example:

"Coffee levels appear critical."

"Assignment deadline detected."

"Productivity probability: 12%."

---

# **FUTURE FEATURES (VERSION 2.0)**

Plan architecture for future upgrades:

* SD Card Storage  
* Notes App  
* To-Do Manager  
* Calendar  
* Alarms  
* Games  
* Sensor Dashboard  
* Voice Commands  
* Bluetooth Connectivity  
* Battery Monitoring  
* OTA Updates

---

# **SOFTWARE REQUIREMENTS**

Platform:

* ESP32  
* Arduino Framework  
* PlatformIO preferred

Libraries:

* Adafruit GFX  
* Adafruit ILI9341  
* WiFi  
* HTTPClient  
* ArduinoJson  
* NTPClient

Provide:

* Folder Structure  
* Class Structure  
* File Organization  
* Modular Design

Example:

src/  
├── main.cpp  
├── display/  
├── menu/  
├── weather/  
├── clock/  
├── calculator/  
├── toolbox/  
├── system/  
└── friday\_core/

---

# **UI REQUIREMENTS**

Theme:

* Cyan  
* Blue  
* White  
* Black

Style:

* Iron Man HUD  
* Sci-Fi Dashboard  
* Minimalist

Animations:

* Boot Animation  
* Menu Rotation  
* Selection Glow  
* Loading Screens  
* Transition Effects

---

# **BOOT SEQUENCE**

Design a cinematic startup sequence:

Screen 1:  
FRIDAY Logo

Screen 2:  
Initializing Systems

Screen 3:  
Connecting WiFi

Screen 4:  
Loading Modules

Screen 5:  
Welcome Message

Example:

"Good Evening User"

"Systems Online"

Then transition into the radial menu.

---

# **DELIVERABLES**

Generate:

1. Complete system architecture  
2. UI wireframes  
3. Firmware architecture  
4. State machine diagrams  
5. Navigation logic  
6. Memory usage estimates  
7. ESP32 optimization techniques  
8. Class diagrams  
9. Folder structure  
10. Development roadmap  
11. MVP implementation plan  
12. Full feature list  
13. Future expansion plan  
14. Recommended libraries  
15. Potential problems and solutions

Think like a professional product designer, embedded engineer, and UI architect building a commercial product.

The final design should feel like a miniature version of Tony Stark's FRIDAY running on an ESP32 with a TFT display.

---

This prompt is detailed enough that most advanced LLMs will generate a full product specification, software architecture, and implementation plan rather than just giving a few feature ideas.

