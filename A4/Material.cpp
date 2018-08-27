// Bradley Pospeck
// Material class implementation

#include <Material.h>	
	
void Material::printMaterial()
{
	cout << "phong: " << phong << '\n';
	cout<< "Ka: \n" << Ka << '\n';
	cout << "Kd: \n" << Kd << '\n';
	cout << "Ks: \n" << Ks << '\n';
	cout << "d: " << d << '\n';
}