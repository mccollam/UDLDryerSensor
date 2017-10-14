union() {
    difference() {
        // Box:
        roundedcube(119, 88, 30, 3);
        translate([2, 2, 4])
            cube([115, 84, 28]);
        
        // LCD cutout:
        translate([7, 8, -2])
            cube([25, 72, 7]);
        // LCD circuit notch:
        // FIXME: make the shape tighter to the actual part (45⁰ angles)
        translate([13, 6, -2])
            cube([12, 3, 7]);
        
        // USB power cable cutout:
        translate([-1, 8, 24])
            rotate([0, 90, 0])
                roundedcube(6, 10, 5, 1);
        
        // Screw holes (LCD):
        translate([4.5, 6.5, -1])
            cylinder(h=6, r=2);
        translate([4.5, 81.5, -1])
            cylinder(h=6, r=2);
        translate([36.5, 6.5, -1])
            cylinder(h=6, r=2);
        translate([36.5, 81.5, -1])
            cylinder(h=6, r=2);
        
        // Screw holes (bottom):
        translate([114.5, 6.5, -1])
            cylinder(h=6, r=2);
        translate([114.5, 81.5, -1])
            cylinder(h=6, r=2);
        
        // Keypad space
        translate([38, 9, -1])
            roundedcube(78, 70, 2, 3);
        
        // Keypad cable cutout
        translate([111, 34, 0])
            cube([3, 20, 6]);
    }

    // LCD support post
    translate([13,80,4])
        cube([12, 3, 3]);
    
/*
    // Text:
    translate([0, 80, 8])
        rotate([90, 0, 270])
            linear_extrude(1)
                text("UDL Dryer 2.0", font="Existence", size=5);
*/
}

module roundedcube(xdim, ydim, zdim, rdim) {
    $fn=64;
    hull() {
        translate([rdim, rdim, 0])
            cylinder(r=rdim, h=zdim);
        translate([xdim-rdim, rdim, 0])
            cylinder(r=rdim, h=zdim);
        translate([rdim, ydim-rdim, 0])
            cylinder(r=rdim, h=zdim);
        translate([xdim-rdim, ydim-rdim, 0])
            cylinder(r=rdim, h=zdim);
    }
}