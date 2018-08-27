// Bradley Pospeck
// Sphere class implementation

#include <Sphere.h>

void Sphere::printSphere()
{
	cout << "cx: " << cx << '\n';
	cout << "cy: " << cy << '\n';
	cout << "cz: " << cz << '\n';
	cout << "radius: " << radius << '\n';
}

void Sphere::setC(const Eigen::Vector3d& eye)
{
	C(0) = cx-eye(0);
	C(1) = cy-eye(1);
	C(2) = cz-eye(2);
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