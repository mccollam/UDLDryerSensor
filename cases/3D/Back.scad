difference() {
    union() {
        // Back:
        roundedcube(119, 88, 4, 3);
        // Lip:
        translate([2, 2, 2])
            cube([115, 84, 3]);
    }

    // Screw holes:
    translate([4.5, 6.5, -1])
        cylinder(h=7, r=2);
    translate([4.5, 81.5, -1])
        cylinder(h=7, r=2);
    translate([36.5, 6.5, -1])
        cylinder(h=7, r=2);
    translate([36.5, 81.5, -1])
        cylinder(h=7, r=2);        
    translate([114.5, 6.5, -1])
        cylinder(h=7, r=2);
    translate([114.5, 81.5, -1])
        cylinder(h=7, r=2);
    
    // Sinks for nuts:
    translate([4.5, 6.5, -1])
        cylinder(h=4, r=3);
    translate([4.5, 81.5, -1])
        cylinder(h=4, r=3);
    translate([36.5, 6.5, -1])
        cylinder(h=4, r=3);
    translate([36.5, 81.5, -1])
        cylinder(h=4, r=3);        
    translate([114.5, 6.5, -1])
        cylinder(h=4, r=3);
    translate([114.5, 81.5, -1])
        cylinder(h=4, r=3);
    
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