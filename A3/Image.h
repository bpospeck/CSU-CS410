// Bradley Pospeck
// Image class declaration
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>

#include <Models.h>
#include <Model.h>
#include <Sphere.h>
#include <Light.h>

using std::ifstream;
using std::string;
using std::cout;
using std::cerr;
using std::stringstream;
using std::max;
using Eigen::Vector3d;
using Eigen::Vector3i;
using Eigen::Matrix3d;

class Image
{
	private:
		// Class collections
		Models* mods;
		Sphere* sphereCollect;
		Light* lightCollect;
		// Camera vars
		Vector3d eye;	// Camera location
		Vector3d look;	// Direction camera is looking
		Vector3d up;	// Vector defining which direction is 'up'
		Vector3d camW;	// Camera's Z-axis, W
		Vector3d camU, camV;
		// Counting objs, spheres and lights
		string models;
		string spheres;
		string lights;
		int numModels  = 0;
		int numSpheres = 0;
		int numLights  = 0;
		// near clipping plane vars
		double focalLength;	// distance from eye/focal point to near clipping plane
		double left, bottom, right, top;	// near clipping plane bounds
		int resWidth, resHeight;	// Image resolution
		// ambient light
		double ambR;
		double ambG;
		double ambB;
		// Intersection vars
		vector<Eigen::Vector3d> pixels;
		vector<Eigen::Vector3d> rays;
		vector<Eigen::Vector3i> rgbPix;
		vector<double> tVals;
		Matrix3d M,M1,M2,M3;
		double beta,gamma;
		double tmin = 1000000.0;
		double tmax = 0.0;
		// Pixel Methods
		double getPx(const int& i);
		double getPy(const int& j);
		// Coloring pixel methods
		void calcSphCol(Sphere& sphere,const Eigen::Vector3d& ray,const double& t, const int& index);
		void calcObjCol(Object3d& object,const Eigen::Vector3d& ray,const double& t, const int& index, const int& faceInd);
	public:
		~Image(); // destructor
		bool isComment(const string& s);
		void readDriver(ifstream& sceneInfo);
		void setCamera();
		void setSpheres();
		void setLights();
		void setModels(const string& dName);
		void setPixels();
		void setRays();
		void findIntersection();
		void setColor();
		void writePpm(const string& ppm);
};

#endif