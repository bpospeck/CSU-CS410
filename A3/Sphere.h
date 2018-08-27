// Bradley Pospeck
// Sphere class declaration
#ifndef SPHERE_H
#define SPHERE_H

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
		Eigen::Matrix3d kd;
		Eigen::Matrix3d ks;
	public:
		double cx,cy,cz; // Sphere center coordinates
		double phong = 16; // phong constant 
		double kaR,kaG,kaB; // ambient light coefficients
		double kdR,kdG,kdB; // diffuse light coefficients
		double ksR,ksG,ksB; // specular light coefficients
		double krR,krG,krB; // attenuation coefficients not used in A3
		double csquared,v; //set by setC and setV below
		double radius; // Sphere radius
		void printSphere();
		void setC(const Eigen::Vector3d& pixel); // set c value for ray-sphere intersection
		void setV(const Eigen::Vector3d& ray); // set v value for ray-sphere intersection
		double rayIntersection();
		void setK(); // Setup matrices for ambient, diffuse, and specular coefficients
		inline Eigen::Matrix3d getKd(){return kd;}
		inline Eigen::Matrix3d getKs(){return ks;}
};

#endif