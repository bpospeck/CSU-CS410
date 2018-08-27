// Bradley Pospeck
// Light class implementation

#include <Light.h>

void Light::printLight()
{
	cout << "Light:\n";
	cout << "x: " << x << '\n';
	cout << "y: " << y << '\n';
	cout << "z: " << z << '\n';
	cout << "w: " << w << '\n';
	cout << "(r,g,b): (" << red << ',' << green << ',' << blue << ")\n";
}

void Light::setLight()
{
	loc(0) = x;
	loc(1) = y;
	loc(2) = z;
	B(0) = red;
	B(1) = green;
	B(2) = blue;
}