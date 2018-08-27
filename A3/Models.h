// Bradley Pospeck
// Models class declaration: Collection of "Model"s
#ifndef MODELS_h
#define MODELS_h

#include <Eigen/Dense>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Model.h>

using std::ifstream;
using std::string;
using std::cout;
using std::stringstream;

class Models
{
	private:
		
	public:
		Models(int);		// Constructor
		~Models();			// Destructor
		// Methods
		int readTransforms(const string& mods);
		int setAxisAngleRot();
		int transform(const string& dir);
		// Variables
		Model* collection;
		int numModels;
};

#endif