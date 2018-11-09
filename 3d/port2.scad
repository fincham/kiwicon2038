difference() {
    union() {
        cylinder(r2=10, r1=12, h=9.17, $fn=64);
        cylinder(r=9, h=10, $fn=64);

        /* seating groove */
        difference() {
            translate([0, 0, 10])
            rotate([0, 0, -4])
                cylinder(r2=6, r1=7, h=2, $fn=64);        

            translate([0, 0, 11.5])
            rotate([0, 0, -4])
                cylinder(r2=6, r1=4, h=1, $fn=64);   
        }
    }
    
    /* bit where the 6.5mm jack goes in */
    translate([0, 0, -0.1])
        cylinder(r=6.4/2, h=25, $fn=50); 
    
    /* grippy bit */
    translate([0, 0, 7.4])
    rotate_extrude(convexity = 10, $fn=64)
    translate([11, 0, 0])
        circle(r = 1);        

    /* vents */
    for (i=[0:15]) {
        rotate([0, 0, i * 24])            
        translate([0, 0, 1])
        rotate([80, 0, 0])
        translate([0, 0, 10])
            cylinder(r=2, h=5, $fn=20);
    }         
}

/* chamfer */
translate([0, 0, 10 - 1])
rotate_extrude(convexity = 10, $fn=64)
translate([9, 0, 0])
    circle(r = 1);        

/* spikey bois */            
for (i=[0:16]) {
    rotate([-5, 0, i * 22.5])
    translate([0, 5, 2])
        cube([1, 2, 10]);
}              

/* bottom fill */
cylinder(r=6.5/2, h=1, $fn=50);
