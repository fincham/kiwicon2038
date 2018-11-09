include </home/fincham/pcb.scad>;

difference() {
    union() {
        cylinder(r2=10, r1=10.5, h=3.17, $fn=64);
        cylinder(r=9, h=4, $fn=64);
        
        /* chamfer */
        translate([0, 0, 2.8])
        rotate_extrude(convexity = 10, $fn=64)
        translate([8.8, 0, 0])
            circle(r = 1.2); 

           
    }
    
    /* bit where the 6.5mm jack goes in */
    translate([0, 0, -0.1])
        cylinder(r=6.4/2, h=25, $fn=50); 
    
    

    
translate([0, 0, 3.8])
scale(0.25)
    pcb_pattern(1,1);    
}


        

difference() {
cylinder(r=5, h=4, $fn=64);
    translate([0, 0, -0.1])
        cylinder(r=6.4/2, h=25, $fn=50); 
}

/* seating groove */
difference() {
    translate([0, 0, 4])
    rotate([0, 0, -4])
        cylinder(r2=6, r1=6.5, h=0.5, $fn=64);        

    translate([0, 0, 3.9])
    rotate([0, 0, -4])
        cylinder(r2=7, r1=4, h=1, $fn=64);   
}   

/* bottom fill */
cylinder(r=6.5/2, h=1, $fn=50);

/* outer rings */
difference() {
    union() {
        scale([1, 1, 0.5])
        translate([0, 0, 0])
        rotate_extrude(convexity = 10, $fn=64)
        translate([11, 0, 0])
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