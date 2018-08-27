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
using Eigen::Vector3d;
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
		string vertexNorms = "";
		string faces = "";
		string matl;	// Only one matl per object
		int numVertices = 0;
		int numFaces = 0;
		int numVertNorms = 0;
		double phong; // Ns in matl file
		double kaR,kaG,kaB;
		double kdR,kdG,kdB;
		double ksR,ksG,ksB;
		double d; // amount of transparency; Used in future assignments, not A3
		// Methods
		string storeComments(ifstream& istr);
		string storeVertices(ifstream& istr);
		string storeVertNorms(ifstream& istr);
		string storeFaces(ifstream& istr);
		void storeMatl(string& s);
		void getVertex(Vector3d& point, int index);
		void getVertNorm(Vector3d& norm, int index);
	public:
		// vars
		Eigen::Matrix3d ka;
		Eigen::Matrix3d kd;
		Eigen::Matrix3d ks;
		static vector<string> repeatObjs;
		static vector<int> numRepeatObjs;
		vector<Eigen::Vector3d> A,B,C; 	  // Points A, B, and C on a Face
		vector<Eigen::Vector3d> Na,Nb,Nc; // Surface normals of Face of points A,B, and C
		MatrixXd vertices;
		MatrixXd norms;
		MatrixXd newVertices;
		MatrixXd newNorms;
		void setObjName(const string& name);
		int convertVertexList();	// Converts 'vertexList' and 'vertexNorms' into matrices
		//~Object3d();//destructor
		void setObjFaces();
		void setMatlProps();
		inline int getNumFaces(){return numFaces;}
		inline double getPhong(){return phong;}
		//inline Eigen::Matrix3d getKa(){return ka;}
		//inline Eigen::Matrix3d getKd(){return kd;}
		//inline Eigen::Matrix3d getKs(){return ks;}
		// Input handlers
		int readObj();							// Read obj file
		bool isBlank(const string& s);			// Check if line is a blank line
		bool isComment(const string& s);		// Check if line is a comment
		bool isV(const string& s);				// Check if line is a vertex description
		bool isVn(const string& s);				// Check if line is a vertex normal description
		bool isS(const string& s);				// Check if line is a smooth shading specification
		bool isF(const string& s);				// Check if line is a face description
		bool isMatl(const string& s);			// Check if line is a material file name
		// Output handlers
		int writeObj(const string& dir);		// Write transformed obj file to direction (dir)
};

#endif