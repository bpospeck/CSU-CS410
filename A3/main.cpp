// Bradley Pospeck CS410
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
	int dot=0;
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
	Image scene;
	scene.readDriver(istr);				// Determine what's in the scene
	scene.setCamera();					// Place the camera in the scene
	string dName = makeDir(argv[1]);
	scene.setSpheres();					// Place spheres (if any) in the scene
	scene.setLights();					// Place lights in the scene
	scene.setModels(dName);				// Place models (if any) in the scene
	scene.setPixels();					// Setup image plane with proper resolution
	scene.setRays();					// Setup the rays shooting out from each pixel
	scene.findIntersection();			// Find closest spheres and objects the rays intersect
	//scene.setColor();					// Color the pixels appropriately
	string ppm = argv[2];
	scene.writePpm(ppm);				// Write out the pixels to a ppm image
	//cout << ppm << '\n';
	return 0;
}