#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Arduino.h>

// ═══════════════════════════════════════════════════
//  FRIDAY — Boot & Transition Animations
// ═══════════════════════════════════════════════════

class Animations {
public:
    // Cinematic 6-stage boot sequence
    static void playCinematicBoot();

private:
    // Individual boot screens
    static void bootPhase1CoreActivation(); // Screen 1: Glowing core
    static void bootPhase2FridayReveal();   // Screen 2: FRIDAY typography
    static void bootPhase3Diagnostics();    // Screen 3: System checks + init
    static void bootPhase4ClockSync();      // Screen 4: WiFi/Time sync + init
    static void bootPhase5AIOnline();       // Screen 5: AI Online
    static void bootPhase6FadeToMenu();     // Screen 6: Fade transition
};

#endif // ANIMATIONS_H
