// Bradley Pospeck
// Material class declaration: Defines object materials
#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Dense>
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::cout;
using Eigen::MatrixXd;
using Eigen::Vector3d;
using std::vector;

class Material
{
	private:

	public:
		// multiple material handling variables
		Eigen::Matrix3d Ka = Eigen::Matrix3d::Zero();
		Eigen::Matrix3d Kd = Eigen::Matrix3d::Zero(); 
		Eigen::Matrix3d Ks = Eigen::Matrix3d::Zero();
		double phong=0;
		double d=0; // multiple d's (transparency, still unused I believe)		
		void printMaterial();
};

#endif