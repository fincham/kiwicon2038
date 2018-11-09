difference() {
    union() {
        cylinder(r2=10, r1=11.3, h=5.17, $fn=64);
        cylinder(r=9, h=6, $fn=64);

        /* seating groove */
        difference() {
            translate([0, 0, 5.5])
            rotate([0, 0, -4])
                cylinder(r2=6, r1=7, h=2, $fn=64);        

            translate([0, 0, 7])
            rotate([0, 0, -4])
                cylinder(r2=6, r1=4, h=1, $fn=64);   
        }
        
        /* chamfer */
        translate([0, 0, 6 - 1])
        rotate_extrude(convexity = 10, $fn=64)
        translate([9, 0, 0])
            circle(r = 1); 

        /* spikey bois */            
        for (i=[0:16]) {
            rotate([-5, 0, i * 22.5])
            translate([0, 5, 2])
                cube([1, 2, 6]);
        }              
    }
    
    /* bit where the 6.5mm jack goes in */
    translate([0, 0, -0.1])
        cylinder(r=6.4/2, h=25, $fn=50); 
    
    /* grippy bit */
    translate([0, 0, 4.5])
    rotate_extrude(convexity = 10, $fn=64)
    translate([11, 0, 0])
        circle(r = 1);      

    translate([0, 0, 2])
    rotate_extrude(convexity = 10, $fn=64)
    translate([11.3, 0, 0])
        circle(r = 1);    
    
    /* random lines */
    translate([-0.25, -10, 5.5])
        cube([0.5, 20, 20]);
    
    rotate([0, 0, 25])
    translate([-0.25, -10, 5.5])
        cube([0.5, 20, 20]);    
        
    translate([-5, -6, 5])
        cylinder(r=0.5, h=20, $fn=8);      
  
  
  /*
    for (i=[0:16]) {
        rotate([5, 0, i * 22.5])
        translate([0, 10.3, 0])
            cube([1, 2,10]);
    }*/
}           

   

/* bottom fill */
cylinder(r=6.5/2, h=1, $fn=50);

/* outer rings */
difference() {
    union() {
        scale([1, 1, 0.5])
        translate([0, 0, 0])
        rotate_extrude(convexity = 10, $fn=64)
        translate([11.3, 0, 0])
            circle(r = 1); 
     
        scale([1, 1, 0.5])
        translate([0, 0, 0])
        rotate_extrude(convexity = 10, $fn=64)
        translate([12, 0, 0])
            circle(r = 0.8);    
    }
    
    translate([-20, -20, -5])
        cube([40, 40, 5]);
}