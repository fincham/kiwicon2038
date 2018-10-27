difference() {
    union() {
        /* fill */
        translate([0, 0, -1])
            cylinder(h=5, r=16);
        
        /* base */
        for (i=[0:8]) {
            rotate([0, 10, i * 40])
                translate([12.5, 0, 5])
                    cube([25,19,5], center=true);
        }
        
        /* stalk */
        difference() {
            rotate([0, 0, -4])
                cylinder(r2=9, r1=13, h=16, $fn=16);
            
            cylinder(r=6.4/2, h=25, $fn=50); // jack
            
            /* first grip ring */
            translate([0, 0, 13])
            rotate_extrude(convexity = 10, $fn=50)
            translate([9.5, 0, 0])
                circle(r = 0.5); 
    
            /* 2nd grip gring */
            translate([0, 0, 11])
            rotate_extrude(convexity = 10, $fn=50)
            translate([10, 0, 0])
                circle(r = 0.5);
        
            /* vents */
            for (i=[0:16]) {
                rotate([0, 10, i * 22.5])            
                translate([0, 0, 6.8])
                rotate([80, 0, 0])
                translate([0, 0, 10])
                    cylinder(r=1.5, h=5, $fn=20);
            }
        }
        
        difference() {
            translate([0, 0, 15])
            rotate([0, 0, -4])
                cylinder(r2=6, r1=9, h=2, $fn=16);        

            translate([0, 0, 16.5])
            rotate([0, 0, -4])
                cylinder(r2=6, r1=4, h=1, $fn=16);   

        }
        
        for (i=[0:16]) {
            rotate([-5, 0, i * 22.5])
            translate([0, 6, 0])
                cube([1, 1, 17]);
        }        
    }
    
    /* flesh ports */
    for (i=[0:8]) {
        rotate([0, 0, i * 40])
        translate([21, 0, -5])
        linear_extrude(height=20)
        rotate([0, 0, 90])
        scale([1.5, 1.5, 1])
            polygon(points = [[-4, -2], [-2, 1], [2, 1], [4, -2]], convexity=2);

    }
    
    translate([0, 0, 5.5])
    rotate_extrude(convexity = 10, $fn=20)
    translate([15.5, 0, 0])
        circle(r = 1);        
    
    
     translate([0, 0, 3])
        cylinder(r=6.4/2, h=25, $fn=50); // jack            
    
     translate([0, 0, 2.5])
        cylinder(r=1, h=25, $fn=50); // jack       
    
    /* cut off */
    translate([-40, -40, -8])
        cube([80, 80, 10]);
}    

