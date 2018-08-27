// Bradley Pospeck
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
}