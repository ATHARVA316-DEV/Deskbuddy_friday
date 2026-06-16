// ============================================================
//  RETRO MAC-STYLE ENCLOSURE
//  For: ESP32 38-pin + 2.4" ILI9341 TFT (portrait)
//
//  Style : 1980s Apple Macintosh 128K — bulky, boxy, chunky
//  Print : FDM — no supports needed in default orientation
//  Split : Part 1 = Top Shell  |  Part 2 = Base Plate
//
//  PRINT ORIENTATION:
//    Top Shell  → stand upright (bottom opening face-down on bed)
//    Base Plate → flat on bed (screw-hole side up)
//
//  ASSEMBLY:
//    1. Solder/connect display wires before closing
//    2. Snap display PCB into front recess slot
//    3. Seat ESP32 onto standoffs, secure with M2.5 screws
//    4. Route Micro-USB cable through back hole
//    5. Drop base plate in, secure 4× M3 screws from below
// ============================================================

$fn = 48;

// ================================================================
//  PARAMETERS  — adjust here if your parts differ slightly
// ================================================================

// --- Outer Shell Dimensions ---
OW  = 85.0;     // Outer Width  (X)
OD  = 52.0;     // Outer Depth  (Y)
OH  = 128.0;    // Outer Height (Z) — tall Mac style
WT  = 2.5;      // Wall thickness (min 2.0 for FDM strength)
CR  = 5.0;      // Corner radius  (keep it boxy, not round)

// --- Base Plate ---
BASE_T  = 5.0;  // Base plate thickness (heavy base = stable)
LIP_H   = 4.0;  // Inner mating lip height

// --- 2.4" ILI9341 TFT Module (portrait) ---
// PCB (the full red board): 42mm × 75mm
// Viewable screen area   : 36.5mm × 48.5mm
SCR_W    = 36.5;   // Screen glass cutout width
SCR_H    = 48.5;   // Screen glass cutout height
BEZEL    = 3.0;    // Raised bezel lip around screen opening
PCB_W    = 43.0;   // Full PCB width  (with tolerance)
PCB_H    = 76.0;   // Full PCB height (with tolerance)
PCB_REC  = 2.0;    // Depth PCB sits into front wall recess
PCB_CLR  = 10.0;   // Clearance depth behind PCB for headers/wires

// Screen window centered on front face
SCR_CX  = OW / 2;
SCR_TOP = OH - 15.0;          // Top of screen from bottom of shell
SCR_BOT = SCR_TOP - SCR_H;   // Bottom of screen
// PCB recess top/bottom (PCB is taller than screen)
PCB_BOT = SCR_BOT - (PCB_H - SCR_H) / 2;

// --- ESP32 38-pin Dev Board ---
// Board size: ~51mm × 28mm
// Mounting hole spacing: ~45mm × 23mm (typical)
ESP_HX  = 22.5;    // Half-spacing X (long axis)
ESP_HY  = 11.5;    // Half-spacing Y (short axis)
ESP_CX  = OW / 2;  // ESP centered in shell width
ESP_CY  = OD / 2;  // ESP centered in shell depth
ESP_STH = 5.0;     // Standoff height (clears bottom PCB components)
ESP_STR = 3.2;     // Standoff outer radius
ESP_HR  = 1.3;     // Screw hole radius (M2.5)

// --- USB Ports (back panel) ---
// Micro-USB: for ESP32 onboard port (programming)
MUSB_W  = 11.5;
MUSB_H  = 7.0;
MUSB_Z  = BASE_T + LIP_H + 6.0;  // From shell bottom

// USB-B: external power/data connector
USBB_W  = 12.5;
USBB_H  = 11.5;
USBB_Z  = MUSB_Z + MUSB_H + 8.0;

// --- Corner Screw Bosses (M3) ---
BOSS_R   = 4.0;
BOSS_HR  = 1.6;    // M3 clearance
BOSS_H   = LIP_H + BASE_T + 2.0;  // Tall enough for base lip
BOSS_OFF = BOSS_R + WT + 0.5;

// --- Speaker Grille ---
DOT_R    = 1.1;
DOT_SPC  = 4.5;
DOTS_COL = 3;
DOTS_ROW = 4;
// Grille center Z (below screen, above label zone)
GRL_CZ   = SCR_BOT - 16.0;

// ================================================================
//  UTILITY MODULES
// ================================================================

// Rounded rectangular box — origin at (0,0,0), extends +X +Y +Z
module rbox(w, d, h, r) {
    r_safe = min(r, w/2 - 0.1, d/2 - 0.1);
    hull() {
        for (xi = [r_safe, w - r_safe])
            for (yi = [r_safe, d - r_safe])
                translate([xi, yi, 0])
                    cylinder(r = r_safe, h = h);
    }
}

// Hollow standoff post
module standoff(h, or, ir) {
    difference() {
        cylinder(r = or, h = h);
        translate([0, 0, -0.1])
            cylinder(r = ir, h = h + 0.5);
    }
}

// ================================================================
//  SHELL BODY  (hollow, open at bottom)
// ================================================================

module shell_body() {
    difference() {

        // ── OUTER FORM ──────────────────────────────────────────
        // Slight taper: back panel leans in 2mm at top (Mac tilt)
        hull() {
            translate([0, 0,      0]) rbox(OW,      OD,      1, CR);
            translate([0, 2, OH - 1]) rbox(OW, OD - 2.0, 1, CR);
        }

        // ── INTERIOR HOLLOW (open bottom) ───────────────────────
        translate([WT, WT, -0.5])
            rbox(OW - 2*WT, OD - 2*WT, OH - WT + 0.5, max(CR - WT, 1.5));

        // ── FRONT FACE: BEZEL STEP ───────────────────────────────
        // Outer bezel recess (gives raised-lip Mac look)
        translate([SCR_CX - SCR_W/2 - BEZEL, -0.5, SCR_BOT - BEZEL])
            cube([SCR_W + 2*BEZEL, WT * 0.6 + 0.5, SCR_H + 2*BEZEL]);

        // ── FRONT FACE: SCREEN GLASS OPENING ────────────────────
        translate([SCR_CX - SCR_W/2, -1, SCR_BOT])
            cube([SCR_W, WT + 2, SCR_H]);

        // ── FRONT FACE: PCB RECESS SLOT ─────────────────────────
        // Module sits flush; 2mm shoulder holds it from falling out
        translate([SCR_CX - PCB_W/2, WT - PCB_REC, PCB_BOT])
            cube([PCB_W, PCB_REC + 0.2, PCB_H]);

        // Header + wire clearance pocket behind PCB
        translate([SCR_CX - (PCB_W - 4)/2, WT, PCB_BOT + 2])
            cube([PCB_W - 4, PCB_CLR, PCB_H - 4]);

        // ── FRONT FACE: SPEAKER GRILLE DOTS ─────────────────────
        for (row = [0 : DOTS_ROW - 1])
            for (col = [0 : DOTS_COL - 1])
                translate([
                    SCR_CX + (col - (DOTS_COL - 1) / 2.0) * DOT_SPC,
                    -1,
                    GRL_CZ - row * DOT_SPC
                ])
                    rotate([-90, 0, 0])
                        cylinder(r = DOT_R, h = WT + 2);

        // ── BACK FACE: MICRO-USB PORT ────────────────────────────
        translate([SCR_CX - MUSB_W/2, OD - WT - 0.5, MUSB_Z])
            cube([MUSB_W, WT + 1, MUSB_H]);

        // ── BACK FACE: USB-B PORT ────────────────────────────────
        translate([SCR_CX - USBB_W/2, OD - WT - 0.5, USBB_Z])
            cube([USBB_W, WT + 1, USBB_H]);

        // ── BACK FACE: CABLE MANAGEMENT SLOT ────────────────────
        // (optional 2mm slot for extra wires)
        translate([SCR_CX - 8, OD - WT - 0.5, USBB_Z + USBB_H + 5])
            cube([16, WT + 1, 5]);
    }
}

// ================================================================
//  INTERNAL FEATURES  (printed in place)
// ================================================================

// ESP32 standoffs at 4 mounting hole positions
module esp32_standoffs() {
    for (xi = [-ESP_HX, ESP_HX])
        for (yi = [-ESP_HY, ESP_HY])
            translate([ESP_CX + xi, ESP_CY + yi, WT + 0.5])
                standoff(ESP_STH, ESP_STR, ESP_HR);
}

// M3 corner screw bosses (shell side)
module corner_bosses() {
    for (xi = [BOSS_OFF, OW - BOSS_OFF])
        for (yi = [BOSS_OFF, OD - BOSS_OFF])
            translate([xi, yi, WT])
                standoff(BOSS_H, BOSS_R, BOSS_HR);
}

// Display PCB retaining clips (stop PCB popping out forward)
// Printed as small overhanging tabs — 45° OK for FDM
module display_clips() {
    clip_h = 6;
    for (sx = [-1, 1]) {
        x_pos = (sx > 0)
            ? SCR_CX + PCB_W/2 + WT - 3
            : SCR_CX - PCB_W/2 - WT;
        translate([x_pos, WT, SCR_BOT + 5])
            difference() {
                cube([3, 3.5, clip_h]);
                // Chamfer front edge so PCB slides in
                translate([-0.1, 3, -0.1])
                    rotate([45, 0, 0])
                        cube([3.2, 3, 3]);
            }
    }
}

// Horizontal ribbing on front face chin (Mac aesthetic)
// Two thin horizontal ridges below the grille
module chin_ribs() {
    rib_z1 = GRL_CZ - DOTS_ROW * DOT_SPC - 5;
    rib_z2 = rib_z1 - 4;
    for (rz = [rib_z1, rib_z2])
        translate([WT + 2, 0, rz])
            cube([OW - 2*WT - 4, 1.2, 1.2]);
}

// ================================================================
//  COMPLETE PARTS FOR EXPORT
// ================================================================

module top_shell() {
    union() {
        shell_body();
        esp32_standoffs();
        corner_bosses();
        display_clips();
        chin_ribs();
    }
}

module base_plate() {
    difference() {
        union() {
            // Thick solid base (heavy = stable, Mac style)
            rbox(OW, OD, BASE_T, CR);

            // Inner mating lip (slides into shell opening)
            translate([WT, WT, BASE_T])
                rbox(OW - 2*WT, OD - 2*WT, LIP_H, max(CR - WT, 1.5));
        }

        // M3 screw clearance holes (match corner bosses)
        for (xi = [BOSS_OFF, OW - BOSS_OFF])
            for (yi = [BOSS_OFF, OD - BOSS_OFF])
                translate([xi, yi, -0.5])
                    cylinder(r = BOSS_HR + 0.2, h = BASE_T + LIP_H + 2);

        // Rubber foot recesses (4 corners, stick-on 8mm pads)
        foot_in = 9;
        for (xi = [foot_in, OW - foot_in])
            for (yi = [foot_in, OD - foot_in])
                translate([xi, yi, -0.5])
                    cylinder(r = 4.2, h = 1.5);
    }
}

// ================================================================
//  RENDER — preview both parts side by side
// ================================================================

// ← Comment these two out when exporting individual STLs
top_shell();
translate([OW + 20, 0, 0])
    base_plate();

// ── TO EXPORT STL ──────────────────────────────────────────────
// 1. Comment the two render lines above
// 2. Uncomment ONE line below
// 3. File → Export → Export as STL
// ---------------------------------------------------------------
// top_shell();     // ← Main Mac body
// base_plate();    // ← Bottom plate
