
// Retro Macintosh-style enclosure
// Units: mm

$fn=48;

// Overall dimensions
body_x = 80;
body_y = 42;
body_z = 55;

wall = 2.5;
base_thickness = 5;

// Split height
split_z = 15;

// Main outer shell with slight Macintosh taper
module outer_body() {
    hull() {
        translate([0,0,0]) cube([80,42,55], center=false);
        translate([2,1,4]) cube([76,40,47], center=false);
    }
}

module screen_cutout() {
    translate([16, 3, 28])
        cube([48, 40, 36]);
}

module usb_cutout() {
    // 12x11 plus tolerance
    translate([33.85, 41, 7])
        cube([12.3, 5, 11.3]);
}

module speaker_holes() {
    for (r=[0:2])
      for (c=[0:3])
        translate([30 + c*5, -1, 12 + r*5])
          rotate([90,0,0])
            cylinder(h=6,d=2.0);
}

module ribs() {
    for (i=[0:5])
      translate([18, -0.5, 8 + i*2.5])
        cube([44,4,0.8]);
}

module top_shell() {
    difference() {
        intersection() {
            outer_body();
            translate([-5,-5,split_z]) cube([100,100,100]);
        }

        translate([wall,wall,split_z])
            cube([body_x-2*wall, body_y-2*wall, body_z]);

        screen_cutout();
        usb_cutout();
        speaker_holes();
    }
}

module bottom_base() {
    difference() {
        intersection() {
            outer_body();
            translate([-5,-5,-5]) cube([100,100,split_z+5]);
        }

        translate([wall,wall,base_thickness])
            cube([body_x-2*wall, body_y-2*wall, split_z]);

        // ventilation slots
        for(i=[0:5])
          translate([15+i*8, 18, -1])
            cube([2,12,2]);
    }

    // ESP32 standoffs (approx)
    for (x=[18,62])
      for (y=[10,32])
        translate([x,y,base_thickness])
        difference() {
            cylinder(h=5,d=6);
            cylinder(h=5,d=2.7);
        }
}

translate([0,0,0]) bottom_base();
translate([90,0,0]) top_shell();
