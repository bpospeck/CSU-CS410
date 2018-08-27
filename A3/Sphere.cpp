// Bradley Pospeck
// Sphere class implementation

#include <Sphere.h>

void Sphere::printSphere()
{
	cout << "Sphere:\n";
	cout << "c(x,y,z): (" << cx << ',' << cy << ',' << cz << ")\n";
	cout << "radius: " << radius << '\n';
	cout << "ka(r,g,b): (" << kaR << ',' << kaG << ',' << kaB << ")\n";
	cout << "kd(r,g,b): (" << kdR << ',' << kdG << ',' << kdB << ")\n";
	cout << "ks(r,g,b): (" << ksR << ',' << ksG << ',' << ksB << ")\n";
	cout << "kr(r,g,b): (" << krR << ',' << krG << ',' << krB << ")\n";
}

void Sphere::setC(const Eigen::Vector3d& pixel)
{
	C(0) = cx-pixel(0);
	C(1) = cy-pixel(1);
	C(2) = cz-pixel(2);
	csquared = C.dot(C);
	//cout << "C and c:" << C << '\n' << c << '\n';
}

void Sphere::setV(const Eigen::Vector3d& ray)
{
	v = C.dot(ray);
	//cout << "C and csquared:" << C << '\n' << csquared << '\n';
	//cout << "ray and v:" << ray << '\n' << v << '\n';
}

double Sphere::rayIntersection()
{
	double dsquared = (radius*radius - (csquared - v*v));
	if(dsquared <0){
		return 100000; //default t in tVals in Image.cpp (no intersection occured)
	}else{
		double d = sqrt(dsquared);
		//cout << "d:" << d << " and t:" << v-d << '\n';
		return v-d;
	}
}

void Sphere::setK()
{
	kd << kdR, 0, 0,
			0,kdG,0,
			0, 0, kdB;
	ks << ksR, 0, 0,
			0,ksG,0,
			0, 0, ksB;
}