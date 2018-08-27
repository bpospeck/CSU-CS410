// Bradley Pospeck
// Model class declaration
#ifndef MODEL_h
#define MODEL_h

#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>

#include <Object3d.h>

using std::ifstream;
using std::string;
using std::cout;
using std::stringstream;
using Eigen::RowVector3d;
using Eigen::Matrix4d;
//using Eigen::Matrix3d;

class Model
{
	private:
		string output = "";
		RowVector3d U,V,W;
		Matrix4d Rw;	// Rotation matrix about given axis
		Matrix4d Rz;	// Rotation matrix about z-axis
		Matrix4d R;		// R= Rw^T * Rz * Rw
		Matrix4d S;		// S is scale matrix
		Matrix4d T;		// T is translation matrix
		Object3d obj;
	public:
		// Methods
		int normalizeW();
		int setUV();
		int setRotMatrix();
		int setTS();
		int populateObj();
		int transformObj(const string& dir);
		// Vars
		double wx;
		double wy;
		double wz;
		double theta;
		double scale;
		double tx;
		double ty;
		double tz;
		string objName;
};

#endif