include </home/fincham/pcb.scad>;

difference() {
    union() {
        cylinder(r2=7, r1=7.5, h=3.17, $fn=64);
cylinder(r2=6.7, r1=6.7, h=3.5, $fn=64);

        /* chamfer */
        translate([0, 0, 3])
        rotate_extrude(convexity = 10, $fn=64)
        translate([6.5, 0, 0])
            circle(r = 0.5); 

           
    }

    for (i=[0:2]) {

rotate([0, 0, 15 * i])
translate([4.1, 4.1, 3])
cylinder(r=0.5, h=25, $fn=50);    
    }
    
    
intersection() {    
    translate([0,0,3])
difference() {
cylinder(r=5.9, h=4, $fn=64);
    translate([0, 0, -0.1])
        cylinder(r=5.8, h=25, $fn=50); 
}        

translate([4, 0, 0])
cube([100,100,100]);
}

    
    /* bit where the 6.5mm jack goes in */
    translate([0, 0, -0.1])
        cylinder(r=6.4/2, h=25, $fn=50); 
    
    translate([0,0,3])
difference() {
cylinder(r=5, h=4, $fn=64);
    translate([0, 0, -0.1])
        cylinder(r=4.8, h=25, $fn=50); 
}    
    

}

        

difference() {
cylinder(r=4.8, h=4, $fn=64);
    translate([0, 0, -0.1])
        cylinder(r2=4.8, r1=3, h=4.5, $fn=50); 
}



/* bottom fill */
cylinder(r=6.5/2, h=1, $fn=50);

/* outer rings */
difference() {
    union() {
        scale([1, 1, 0.5])
        translate([0, 0, 0])
        rotate_extrude(convexity = 10, $fn=64)
        translate([7.5, 0, 0])
            circle(r = 1); 
     /*
        scale([1, 1, 0.5])
        translate([0, 0, 0])
        rotate_extrude(convexity = 10, $fn=64)
        translate([12, 0, 0])
            circle(r = 0.8);    */
    }
    
    translate([-20, -20, -5])
        cube([40, 40, 5]);
}