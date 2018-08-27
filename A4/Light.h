// Bradley Pospeck
// Light class declaration
#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>

using std::cout;

class Light
{
	private:
		Eigen::Vector3d loc; // coordinates of light
		Eigen::Vector3d B; // Color of light
	public:
		double x,y,z,w; // location of light source
		double red,green,blue; // Color of light source
		// Methods
		void printLight(); // mainly for debugging
		void setLight();
		inline Eigen::Vector3d getLoc(){return loc;}
		inline Eigen::Vector3d getB(){return B;}
};

#endif