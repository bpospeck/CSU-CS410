// Bradley Pospeck
// Sphere class declaration

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>
#include <math.h>

using std::cout;

class Sphere
{
	private:
		Eigen::Vector3d C;
	public:
		// Sphere center coordinates
		double cx;
		double cy;
		double cz;
		double csquared,v; //set by setC and setV below
		double radius; // Sphere radius
		void printSphere();
		void setC(const Eigen::Vector3d& eye); // set c value for ray-sphere intersection
		void setV(const Eigen::Vector3d& ray); // set v value for ray-sphere intersection
		double rayIntersection();
};