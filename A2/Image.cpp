main.cpp                                                                                            0000600 0004030 0003025 00000002476 13167200204 012176  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck CS410
// Main code to read in driver file

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>

#include <Model.h>
#include <Models.h>
#include <Image.h>

string makeDir(string dirName)
{
	int dot;
	for(unsigned int i=0; i<dirName.length(); i++){
		if(dirName[i] == '.'){
			dot = i;
			break;
		}
	}
	dirName = dirName.substr(0,dot);
	string mkdir = "mkdir ";
	mkdir.append(dirName); // Full linux command ready
	//Apparently need to convert string to 'const char*'
	//const char* command = mkdir.c_str();
	//system(command); Don't need to make a directory in A2, this is a quick fix
	return dirName;
}

int main(int argc, char* argv[])
{	
	// Commonly used pieces in main
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::string;
	// Make sure only the driver file was passed as an argument
	if (argc != 3){
		cerr << "2 arguments expected" << endl;
		return -1;
	}
	std::ifstream istr(argv[1]);
	Image image;
	image.readDriver(istr);
	image.setCamera();
	string dName = makeDir(argv[1]);
	image.setSpheres();
	image.setModels(dName);
	image.setPixels();
	image.setRays();
	image.findIntersection();
	image.setColor();
	string ppm = argv[2];
	image.writePpm(ppm);
	//cout << ppm << '\n';
	return 0;
}                                                                                                                                                                                                  Model.cpp                                                                                           0000600 0004030 0003025 00000004607 13167173607 012330  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
// Model class implementation

#include <Model.h>	
	
int Model::normalizeW()
{
	W(0) = wx;
	W(1) = wy;
	W(2) = wz;
	W.normalize();
	//cout << "W:" << W[0] << W[1] << W[2] << '\n';
	return 0;
}

int Model::setUV()
{
	// Pick smallest value of W and replace with 1 
	int minW = 0; // Index of min value in W
	for(int i=1; i<3; i++){ // W always 3D i believe, so hard coded 3
		if(fabs(W[i]) <  W[minW]) minW = i;
	}
	RowVector3d M = W;
	//cout << "M:" << M[0] << " " << M[1] << " " << M[2] << '\n';
	M[minW] = 1;
	//cout << "Malt:" << M[0] << " " << M[1] << " " << M[2] << '\n';
	M.normalize();
	U = W.cross(M);
	U.normalize();
	V = W.cross(U);
	//V.normalize();
	//cout << "U:" << U[0] << " " << U[1] << " " << U[2] << '\n';
	//cout << "V:" << V[0] << " " << V[1] << " " << V[2] << '\n';
	//cout << "M:" << M[0] << " " << M[1] << " " << M[2] << '\n';
	return 0;
}

int Model::setRotMatrix()
{
	// Build Rw, Rz, and R here
	Rw = Matrix4d::Zero();
	//cout << "Rw matrix:\n" << Rw << '\n';
	Rw(3,3) = 1; // May need to be scale value, not always 1
	// Set upper left 3x3 Rot matrix to U,V,W in each row
	for(int row = 0; row < Rw.rows()-1 ; row++){
		for(int col = 0; col <Rw.cols()-1 ; col++){
			if(row == 0){
				Rw(row,col) = U[col];
			}else if(row ==1){
				Rw(row,col) = V[col];
			}else{
				Rw(row,col) = W[col];
			}
		}
	}
	// Set Rz matrix (rotation about z-axis)
	Rz << cos(theta), -sin(theta), 0, 0,
		  sin(theta), cos(theta),  0, 0,
		  0,		  0,		   1, 0,
		  0,		  0,		   0, 1;
	//cout << "Rw:\n";
	//cout << Rw << '\n';
	//cout << "Rz:\n";
	//cout << Rz << '\n';
	//cout << "Rw^T:\n";
	//cout << Rw.transpose() << '\n';
	R = Rw.transpose() * Rz * Rw;
	//cout << "Final rotation transform matrix:\n";
	//cout << R << "\n";
	return 0;
}

int Model::setTS()
{
	S << scale,0,     0,    0,
		 0,	   scale, 0,    0,
		 0,	   0,     scale,0,
		 0,    0,     0,    1;
	T << 1,0,0,tx,
		 0,1,0,ty,
		 0,0,1,tz,
		 0,0,0,1;
	//cout << "S:\n" << S << '\n';
	//cout << "T:\n" << T << '\n';
	return 0;
}

int Model::populateObj()
{
	obj.setObjName(objName);
	obj.readObj();
	obj.convertVertexList();
	return 0;
}

int Model::transformObj(const string& dir)
{
	obj.newVertices = T*S*R*obj.vertices;
	//cout << "transformed vertices:\n" << obj.newVertices << "\n";
	obj.setObjFaces();
	//obj.writeObj(dir);
	return 0;
}                                                                                                                         Models.cpp                                                                                          0000600 0004030 0003025 00000004043 13167000107 012465  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
// Models class implementation

#include <Models.h>

Models::Models(int numModels) // Constructor implementation
{
	collection = new Model[numModels];
}

Models::~Models()				// Destructor Implementation
{
	delete[] collection;
}

int Models::readTransforms(const string& mods)
{
	//string test;
	double val;
	int index = 0;
	stringstream ss;
	ss << mods;
	while( !ss.eof() ){
		// Store doubles into Model matrices
		// Store .obj file name in Model var
		if ( !(ss >> val) ){
			break;
		}
		collection[index].wx = val;
		ss >> collection[index].wy;
		ss >> collection[index].wz;
		ss >> collection[index].theta; // Need to convert theta to radians
		collection[index].theta *= (3.14159265/180);
		ss >> collection[index].scale;
		ss >> collection[index].tx;
		ss >> collection[index].ty;
		ss >> collection[index].tz;
		ss >> collection[index].objName;
		index++;
	}
	numModels = index;
	/*cout << numModels << '\n';
	for(int i=0; i<numModels; i++){
		cout << "Model #" << i << ":";
		cout << collection[i].wx << ", ";
		cout << collection[i].wy << ", ";
		cout << collection[i].wz << ", ";
		cout << collection[i].theta << ", ";
		cout << collection[i].scale << ", ";
		cout << collection[i].tx << ", ";
		cout << collection[i].ty << ", ";
		cout << collection[i].tz << ", ";
		cout << collection[i].objName << "\n";
	}*/
	return 0;
}

int Models::setAxisAngleRot()
{
	/* One time, set size of static vars:
	   repeatObjs, and numRepeatObjs
	   they're used to keep track of object duplicates
	   when writing a new obj out to a file*/
	//collection[0].initRepeats(numModels);
	for(int i=0; i<numModels; i++){
		collection[i].normalizeW(); // Normalize each model's W
		collection[i].setUV();		// Set U, V normalized axes from W
		collection[i].setRotMatrix();
		///cout << '\n';
	}
	return 0;
}

int Models::transform(const string& dir)
{
	for(int i=0; i<numModels; i++){
		collection[i].setTS();
		collection[i].populateObj();
		collection[i].transformObj(dir);
	}
	return 0;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             Object3d.cpp                                                                                        0000600 0004030 0003025 00000016430 13167060424 012712  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
// Object3d class implementation

#include <Object3d.h>

//Static vars
vector<string> Object3d::repeatObjs;
vector<int> Object3d::numRepeatObjs;

void Object3d::setObjName(const string& name)
{
	int fileExtension;
	for(unsigned int i=0; i<name.length(); i++){
		if(name[i] == '.') fileExtension = i;
	}
	objName = name.substr(0,fileExtension);
	// Prepare static vars about repeat objects
	bool foundRep = false;
	if(repeatObjs.size() == 0){
		repeatObjs.push_back(objName);
		numRepeatObjs.push_back(1);
	}else{
		for(unsigned int i=0; i<repeatObjs.size(); i++){
			if(objName == repeatObjs[i]){
				numRepeatObjs[i] += 1;
				foundRep=true;
				break;
			}
		}
		if(!foundRep){
			repeatObjs.push_back(objName);
			numRepeatObjs.push_back(1);
		}
	}
	//Testing for loop
	//cout << "diff obj\n";
	//for(unsigned int i=0; i<repeatObjs.size(); i++){
	//	cout << "obj:" << repeatObjs[i] << " numRep:" << numRepeatObjs[i] << '\n';
	//}
}

string Object3d::storeComments(ifstream& istr)
{
	string line; 	 // Will be line by line parsing of driver file
	while(getline(istr,line)){
		if(isComment(line)){ 
			comments.append(line);
			comments.append("\n");
		}else if(isBlank(line)){
			continue;
		}else{ 
			return line;
		}
	}
	return "";
}

string Object3d::storeVertices(ifstream& istr)
{
	string line;
	while(getline(istr,line)){
		if(isV(line)){
			vertexList.append(line);
			vertexList.append("\n");
			numVertices++;
		}else if(isBlank(line) || isComment(line) || isS(line) || isVn(line)){
			continue;
		}else{ // Should be end of vertices; return line
			return line;
		}
	}
	return "";
}

string Object3d::storeFaces(ifstream& istr)
{
	string line;
	while(getline(istr,line)){
		if(isF(line)){
			faces.append(line);
			faces.append("\n");
			numFaces++;
		}else if(isBlank(line) || isComment(line) || isS(line) || isVn(line)){
			continue;
		}else{
			return line;
		}
	}
	return "";
}

void Object3d::getVertex(Vector3d& point, int index)
{
	for(int i=0; i<3; i++){
		point(i) = newVertices(i,index);
	}
	//cout << point << "\n";
}

void Object3d::setObjFaces()
{
	stringstream ss;
	ss << faces;
	string scratch;
	int val;
	string delimeter = "//";
	ss >> scratch;
	Vector3d triangle; 
	while( !ss.eof()){
		if( scratch != "f"){
			break;
		}
		// Point A
		ss >> scratch;
		scratch = scratch.substr(0, scratch.find(delimeter));
		val = std::stoi(scratch);
		//cout << "A:" << val << '\n';
		getVertex(triangle, val-1); // Faces are 1 index, not 0 so subtract 1
		A.push_back(triangle);
		// Point B
		ss >> scratch;
		scratch = scratch.substr(0, scratch.find(delimeter));
		val = std::stoi(scratch);
		//cout << "B:" << val << '\n';
		getVertex(triangle, val-1); // Faces are 1 index, not 0 so subtract 1
		B.push_back(triangle);
		// Point C
		ss >> scratch;
		scratch = scratch.substr(0, scratch.find(delimeter));
		val = std::stoi(scratch);
		//cout << "C:" << val << '\n';
		getVertex(triangle, val-1); // Faces are 1 index, not 0 so subtract 1
		C.push_back(triangle);
		// Should be 'f' of next line
		ss >> scratch;
	}		
}

int Object3d::readObj()						// Read obj file
{
	string fName = objName;
	fName.append(".obj");
	ifstream objInfo(fName);
	string nextLine;
	nextLine = storeComments(objInfo);
	while(!objInfo.eof()){
		if(isV(nextLine)){
			vertexList.append(nextLine);
			vertexList.append("\n");
			numVertices++;
			nextLine = storeVertices(objInfo);
		}else if(isBlank(nextLine)){
			getline(objInfo,nextLine);
		}else if(isVn(nextLine)){
			getline(objInfo,nextLine);
		}else if(isS(nextLine)){
			getline(objInfo,nextLine);
		}else if(isF(nextLine)){ 
			faces.append(nextLine);
			faces.append("\n");
			numFaces++;
			nextLine = storeFaces(objInfo);
		}else{
			//isV or isF may have run out of lines and returned nothing to nextLine
			continue;
		}
	}
	// Don't believe any faces or vertices should be left here, but just in case..
	if(isV(nextLine)){
		vertexList.append(nextLine);
		vertexList.append("\n");
		numVertices++;
	}else if(isF(nextLine)){
		faces.append(nextLine);
		faces.append("\n");
		numFaces++;
	}
	//cout << "Name:" << objName << "\n";
	//cout << "numVertices:" << numVertices << "\n";
	//cout << "numFaces:" << numFaces << "\n";
	//cout << comments << vertexList << faces;
	return 0;
}

int Object3d::convertVertexList()
{
	stringstream ss;
	char c;
	ss << vertexList;
	vertices.resize(4,numVertices);
	newVertices.resize(4,numVertices);
	//cout << vertices.rows() << " and cols:" << vertices.cols() << "\n";
	for(int col=0; col<vertices.cols(); col++){
		ss >> c;
		for(int row=0; row<vertices.rows(); row++){
			if(!ss.eof()){
				if(row !=3){
					ss >> vertices(row,col);
				}else{
					vertices(row,col) = 1;
				}
			}
		}
	}
	//cout << "verticesMatrix:\n" << vertices << "\n";
	// Now that everything is setup, write new obj to file
	return 0;
}

bool Object3d::isBlank(const string& s)		// Check if line is a blank line
{
	for(unsigned int i=0; i<s.length(); i++){
		if(!isspace(s[i])) return false;
	}
	return true;
}

bool Object3d::isComment(const string& s)		// Check if line is a comment
{
	for (unsigned int i=0; i<s.length(); i++){
		if(s[i] == '#') return true;
		if(!isspace(s[i])) return false;
	}
	return false;
}

bool Object3d::isV(const string& s)			// Check if line is a vertex description
{
	for(unsigned int i=0; i<s.length(); i++){
		if(s[i] == 'v' && s[i+1] != 'n') return true;
		if(!isspace(s[i])) return false;
	}
	return false;
}

bool Object3d::isVn(const string& s)			// Check if line is a vertex normal description
{
	for(unsigned int i=0; i<s.length(); i++){
		if(s[i] == 'v' && s[i+1] == 'n') return true;
		if(!isspace(s[i])) return false;
	}
	return false;
}

bool Object3d::isS(const string& s)			// Check if line is a smooth shading specification
{
	for(unsigned int i=0; i<s.length(); i++){
		if(s[i] == 's') return true;
		if(!isspace(s[i])) return false;
	}
	return false;
}

bool Object3d::isF(const string& s)			// Check if line is a face description
{
	for(unsigned int i=0; i<s.length(); i++){
		if(s[i] == 'f') return true;
		if(!isspace(s[i])) return false;
	}
	return false;
}

int Object3d::writeObj(const string& dir)					// Write transformed obj file
{
	ofstream oFile;
	string temp = objName;
	int repeat;
	for(unsigned int i=0; i<repeatObjs.size(); i++){
		if(objName == repeatObjs[i]){
			repeat = numRepeatObjs[i];
			break;
		}
	}
	string version = "_mw";
	if(repeat <= 10){
		version.append("0");
		version.append(to_string(repeat-1));
	}else{
		version.append(to_string(repeat-1));
	}
	version.append(".obj"); //Add file extension
	objName.append(version); // Complete file name needed
	//cout << "File name:" << objName << "\n";
	string fPath = dir; // path to file
	fPath.append("/");
	fPath.append(objName);
	oFile.open(fPath);
	if(oFile){
		oFile << comments << "# transformed version of " << temp << ".obj\n";
	}
	for(int col=0; col<vertices.cols(); col++){
		if(oFile) oFile << "v ";
		for(int row=0; row<3; row++){
			if(oFile){
				oFile << std::fixed << std::setprecision(7) << newVertices(row,col) << " ";
			}
		}
		if(oFile) oFile << "\n";
	}
	if(oFile){
		oFile << faces << "\n";
	}
	return 0;
}                                                                                                                                                                                                                                        Sphere.cpp                                                                                          0000600 0004030 0003025 00000001572 13167226312 012504  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
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
}                                                                                                                                      Image.h                                                                                             0000600 0004030 0003025 00000003135 13167176164 011753  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
// Image class declaration

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>

#include <Models.h>
#include <Model.h>
#include <Sphere.h>

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
		Models* mods;
		Sphere* sphereCollect;
		// Camera vars
		Vector3d eye;	// Camera location
		Vector3d look;	// Direction camera is looking
		Vector3d up;	// Vector defining which direction is 'up'
		Vector3d camW;	// Camera's Z-axis, W
		Vector3d camU, camV;
		// Counting objs and spheres
		string models;
		string spheres;
		int numModels = 0;
		int numSpheres = 0;
		// near clipping plane vars
		double focalLength;	// distance from eye/focal point to near clipping plane
		double left, bottom, right, top;	// near clipping plane bounds
		int resWidth, resHeight;	// Image resolution
		// Intersection vars
		vector<Eigen::Vector3d> pixels;
		vector<Eigen::Vector3d> rays;
		vector<Eigen::Vector3i> rgbPix;
		vector<double> tVals;
		Matrix3d M,M1,M2,M3;
		double beta,gamma;
		double tmin = 1000000.0;
		double tmax = 0.0;
		//Methods
		double getPx(const int& i);
		double getPy(const int& j);
	public:
		~Image(); // destructor
		bool isComment(const string& s);
		void readDriver(ifstream& sceneInfo);
		void setCamera();
		void setSpheres();
		void setModels(const string& dName);
		void setPixels();
		void setRays();
		void findIntersection();
		void setColor();
		void writePpm(const string& ppm);
};                                                                                                                                                                                                                                                                                                                                                                                                                                   Model.h                                                                                             0000600 0004030 0003025 00000002003 13167057752 011763  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
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
	public:
		// Methods
		int normalizeW();
		int setUV();
		int setRotMatrix();
		int setTS();
		int populateObj();
		int transformObj(const string& dir);
		// Vars
		Object3d obj;
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

#endif                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             Models.h                                                                                            0000600 0004030 0003025 00000001151 13160075435 012140  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
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

#endif                                                                                                                                                                                                                                                                                                                                                                                                                       Object3d.h                                                                                          0000600 0004030 0003025 00000003460 13167064344 012363  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
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
		string faces = "";
		int numVertices = 0;
		int numFaces = 0;
		// Methods
		string storeComments(ifstream& istr);
		string storeVertices(ifstream& istr);
		string storeFaces(ifstream& istr);
		void getVertex(Vector3d& point, int index);
	public:
		// vars
		static vector<string> repeatObjs;
		static vector<int> numRepeatObjs;
		vector<Eigen::Vector3d> A,B,C; // Points A, B, and C on a Face
		MatrixXd vertices;
		MatrixXd newVertices;
		void setObjName(const string& name);
		int convertVertexList();
		//~Object3d();//destructor
		void setObjFaces();
		inline int getNumFaces(){return numFaces;}
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

#endif                                                                                                                                                                                                                Sphere.h                                                                                            0000600 0004030 0003025 00000001145 13167226322 012146  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  // Bradley Pospeck
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
};                                                                                                                                                                                                                                                                                                                                                                                                                           README.txt                                                                                          0000600 0004030 0003025 00000000345 13167226756 012261  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  Bradley Pospeck
CS410 A2

Unfortunately, I had it in my mind that the assignment was due a week later.
So, I didn't have enough time to finish it.

To compile, simply use 'make'. To run it: "./raytracer driver##.txt driver##.ppm"                                                                                                                                                                                                                                                                                           makefile                                                                                            0000600 0004030 0003025 00000001123 13167227013 012241  0                                                                                                    ustar   bpospeck                        under                                                                                                                                                                                                                  # CS410 makefile
# Bradley Pospeck

CC=g++
OBJS=main.o Model.o Models.o Object3d.o Image.o Sphere.o
CFLAGS=-Wall -I. -c -std=c++14
LFLAGS=-Wall

all: raytracer
raytracer: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o raytracer

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Model.o: Model.cpp Model.h
	$(CC) $(CFLAGS) Model.cpp
	
Models.o: Models.cpp Models.h
	$(CC) $(CFLAGS) Models.cpp
	
Object3d.o: Object3d.cpp Object3d.h
	$(CC) $(CFLAGS) Object3d.cpp
	
Image.o: Image.cpp Image.h
	$(CC) $(CFLAGS) Image.cpp

Sphere.o: Sphere.cpp Sphere.h
	$(CC) $(CFLAGS) Sphere.cpp

clean:
	rm -f $(OBJS) *~ raytracer
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             