$fn=64;
difference() {
    roundedsquare(119, 88, 3);
    
    // Screw holes (LCD):
    translate([4.5, 6.5])
        circle(r=2);
    translate([4.5, 81.5])
        circle(r=2);
    translate([35.5, 6.5])
        circle(r=2);
    translate([35.5, 81.5])
        circle(r=2);
    
    // Screw holes (bottom):
    translate([114.5, 6.5])
        circle(r=2);
    translate([114.5, 81.5])
        circle(r=2);
        
}

module roundedsquare(xdim, ydim, rdim) {
    $fn=64;
    hull() {
        translate([rdim, rdim, 0])
            circle(r=rdim);
        translate([xdim-rdim, rdim, 0])
            circle(r=rdim);
        translate([rdim, ydim-rdim, 0])
            circle(r=rdim);
        translate([xdim-rdim, ydim-rdim, 0])
            circle(r=rdim);
    }
}