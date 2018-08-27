// Bradley Pospeck
// Object3d class declaration: Defines 3d object from inputs
#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <Eigen/Dense>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

using std::string;
using std::ifstream;
using std::ofstream;
using Eigen::MatrixXd;
using std::cout;
using std::stringstream;
using std::vector;
using std::to_string;

class Object3d
{
	private:
		// variables
		string objName;
		string comments = "";
		string vertexList = "";
		string faces = "";
		int numVertices = 0;
		int numFaces = 0;
		// Methods
		string storeComments(ifstream& istr);
		string storeVertices(ifstream& istr);
		string storeFaces(ifstream& istr);
	public:
		// vars
		static vector<string> repeatObjs;
		static vector<int> numRepeatObjs;
		MatrixXd vertices;
		MatrixXd newVertices;
		void setObjName(const string& name);
		int convertVertexList();
		//~Object3d();//destructor
		// Input handlers
		int readObj();							// Read obj file
		bool isBlank(const string& s);			// Check if line is a blank line
		bool isComment(const string& s);		// Check if line is a comment
		bool isV(const string& s);				// Check if line is a vertex description
		bool isVn(const string& s);				// Check if line is a vertex normal description
		bool isS(const string& s);				// Check if line is a smooth shading specification
		bool isF(const string& s);				// Check if line is a face description
		// Output handlers
		int writeObj(const string& dir);		// Write transformed obj file to direction (dir)
};

#endif