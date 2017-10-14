$fn=64;
union() {
    difference() {
        // Box:
        roundedcube(119, 88, 24, 3);
        translate([2, 2, 4])
            cube([115, 84, 28]);
        
        // LCD cutout:
        translate([7, 8, -2])
            cube([25, 72, 7]);
        // LCD circuit notch:
        translate([13, 4.5, 1])
            cube([12, 4.5, 7]);
        
        // USB power cable cutout:
        translate([-1, 14, 20])
            rotate([0, 90, 0])
                roundedcube(6, 10, 5, 1);
        
        // Screw holes (LCD):
        translate([4.5, 6.5, -1])
            cylinder(h=6, r=2);
        translate([4.5, 81.5, -1])
            cylinder(h=6, r=2);
        translate([35.5, 6.5, -1])
            cylinder(h=6, r=2);
        translate([35.5, 81.5, -1])
            cylinder(h=6, r=2);
        
        // Screw holes (bottom):
        translate([114.5, 6.5, -1])
            cylinder(h=6, r=2);
        translate([114.5, 81.5, -1])
            cylinder(h=6, r=2);
        
        // Screw head sinks (LCD):
        translate([4.5, 6.5, -1])
            cylinder(h=3, r=2.5);
        translate([4.5, 81.5, -1])
            cylinder(h=3, r=2.5);
        translate([36.5, 6.5, -1])
            cylinder(h=3, r=2.5);
        translate([36.5, 81.5, -1])
            cylinder(h=3, r=2.5);
        // Screw head sinks (bottom):
        translate([114.5, 6.5, -1])
            cylinder(h=3, r=2.5);
        translate([114.5, 81.5, -1])
            cylinder(h=3, r=2.5);

        // Keypad space
        translate([38, 9, -1])
            roundedcube(78, 70, 2, 3);
        
        // Keypad cable cutout
        translate([111, 33, -1])
            cube([3, 22, 7]);
            
        // Text
        linear_extrude(.5)
            translate([5, 70, -1])
                rotate([0, 180, 90])
                    text("UDL Dryer 2.0", font="Existence", size=3);
    }
 
    // LCD support post
    translate([13,80,4])
        cube([12, 7, 3]);

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