// Bradley Pospeck CS410
// Main code to read in driver file

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>

#include <Model.h>
#include <Models.h>

bool isComment(const string& s)
{
	for (unsigned int i = 0; i < s.length(); i++) {    
        if (s[i] == '#') return true;	
        if ( ! isspace(s[i]) ) return false;
    }
    return false;
}

int readDriver(ifstream& modelInfo, string& models)
{
	string line; 	 // Will be line by line parsing of driver file
	string scratch;
	double num;
	int numModels = 0;
	stringstream ss; // Will be used to check each line value by value
	if(modelInfo.is_open()){
		while(getline(modelInfo,line)){
			if(isComment(line)) continue;
			ss << line;
			while( !ss.eof()){
				if (!(ss >> num)){
					ss.clear();
					if( ss >> scratch){
						if(scratch != "model") models.append(scratch);
					}
				}else{
					models.append(std::to_string(num));
				}
				models.append(" ");
			}
			ss.str("");
			ss.clear();
			numModels++;
		}
		modelInfo.close();
	}else{
		// Error comment here for unopened file
		return -1;
	}
	return numModels;
}

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
	const char* command = mkdir.c_str();
	system(command);
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
	if (argc != 2){
		cerr << "Only 1 argument expected" << endl;
		return -1;
	}
	std::ifstream istr(argv[1]);
	string m = "";
	int numModels = readDriver(istr, m);
	if(numModels == -1) return -1;
	Models models(numModels); // Class that handles multiple models
	models.readTransforms(m);
	models.setAxisAngleRot();
	string dName = makeDir(argv[1]);
	models.transform(dName);		  // Handles transformation and file output
	return 0;
}