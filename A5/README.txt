Bradley Pospeck
CS410 A4

To compile, simply use 'make'. To run it: "./raytracer driver##.txt driver##.ppm"

driver01.txt Is simply 2 checkerboards side by side (one red, one green)
with a single sphere on top showing refrection

driver02.txt has 3 checkerboards laid out horizontally with 3 spheres on top.
One of the checkerboards has each colored square split between 2 color triangles.
2 flat surfaces (checkerboards) are vertically butted up against the 3
horizontal checkerboards in order to essentially make a corner of a room. 
No lights shine directly on the vertical surfaces, so they act like mirrors.
The sphere in the middle has no diffuse light. It mostly just refracts 
and reflects equally with a ko of 0.5. The sphere on the front left is 
purely refractive with an eta of 0.8 to break physics, which looks interesting. The sphere on the
front right is purely a white reflective sphere.