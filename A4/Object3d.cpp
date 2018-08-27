// Bradley Pospeck
// Object3d class implementation

#include <Object3d.h>

//Static vars
vector<string> Object3d::repeatObjs;
vector<int> Object3d::numRepeatObjs;

void Object3d::setObjName(const string& name)
{
	int fileExtension=0;
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

string Object3d::storeVertNorms(ifstream& istr)
{
	string line;
	while(getline(istr,line)){
		if(isVn(line)){
			vertexNorms.append(line);
			vertexNorms.append("\n");
			numVertNorms++;
		}else if(isBlank(line) || isComment(line) || isS(line)){
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
		}else if(isBlank(line) || isComment(line) || isS(line)){
			continue;
		}else{ // Should be end of vertices; return line
			return line;
		}
	}
	return "";
}

string Object3d::storeFaces(ifstream& istr,const int& whichMatl)
{
	string line;
	while(getline(istr,line)){
		if(isF(line)){
			faces.append(line);
			faces.append("\n");
			numFaces++;
			matlUseCount[whichMatl]++;
		}else if(isBlank(line) || isComment(line) || isS(line)){
			continue;
		}else{
			return line;
		}
	}
	return "";
}

void Object3d::storeMatl(string& s)
{
	for(unsigned int i=0; i<s.length(); i++){
		if(s[i] == 'm'){
			if(s.substr(i,6) == "mtllib"){
				int index = i+6;
				matlName = s.substr(index,s.length()-index);
				break;
			} 
		}
	}
	while(true){
		int size = matlName.length();
		if(isspace(matlName[0])){
			matlName = matlName.substr(1, size-1);
		}else if(isspace(matlName[size-1])){
			matlName = matlName.substr(0, size-1);
		}else{
			break; //matlName has no more whitespace
		}
	}
	//cout << "material file name: " << matlName << '\n';
}

void Object3d::getVertex(Vector3d& point, int index)
{
	for(int i=0; i<3; i++){
		point(i) = newVertices(i,index);
	}
	//cout << "point:" << point << "\n";
}

void Object3d::getVertNorm(Vector3d& norm, int index)
{
	for(int i=0; i<3; i++){
		norm(i) = newNorms(i,index);
	}
	norm.normalize();
	//cout << "normal:\n" << norm << '\n';
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
	Vector3d normal;
	while( !ss.eof()){
		if( scratch != "f"){
			break;
		}
		// Point A
		ss >> scratch;
		//scratch = scratch.substr(0, scratch.find(delimeter));
		//val = std::stoi(scratch);
		val = std::stoi(scratch.substr(0, scratch.find(delimeter)));
		getVertex(triangle, val-1); // Faces use 1 index, not 0 so subtract 1
		A.push_back(triangle);
		val = std::stoi(scratch.substr(scratch.find(delimeter)+2, scratch.length()));
		getVertNorm(normal, val-1); // Faces use 1 index, not 0 so subtract 1
		Na.push_back(normal);
		// Point B
		ss >> scratch;
		//scratch = scratch.substr(0, scratch.find(delimeter));
		//val = std::stoi(scratch);
		val = std::stoi(scratch.substr(0, scratch.find(delimeter)));
		getVertex(triangle, val-1); // Faces are 1 index, not 0 so subtract 1
		B.push_back(triangle);
		val = std::stoi(scratch.substr(scratch.find(delimeter)+2, scratch.length()));
		getVertNorm(normal, val-1); // Faces use 1 index, not 0 so subtract 1
		Nb.push_back(normal);
		// Point C
		ss >> scratch;
		//scratch = scratch.substr(0, scratch.find(delimeter));
		//val = std::stoi(scratch);
		val = std::stoi(scratch.substr(0, scratch.find(delimeter)));
		getVertex(triangle, val-1); // Faces are 1 index, not 0 so subtract 1
		C.push_back(triangle);
		val = std::stoi(scratch.substr(scratch.find(delimeter)+2, scratch.length()));
		getVertNorm(normal, val-1); // Faces use 1 index, not 0 so subtract 1
		Nc.push_back(normal);
		// Should be 'f' of next line
		ss >> scratch;
	}
}

void Object3d::setMatlProps()
{
	Material material;
	ifstream matlInfo(matlName);
	string nextLine; 
	string matlStream = "";
	getline(matlInfo,nextLine);
	while(!matlInfo.eof()){
		if(isComment(nextLine)){
			getline(matlInfo,nextLine);
		}else if(isBlank(nextLine)){
			getline(matlInfo,nextLine);
		}else{
			matlStream.append(nextLine);
			matlStream.append("\n");
			getline(matlInfo,nextLine);
		}
	}
	//cout << matlStream;
	stringstream ss;
	ss << matlStream;
	string scratch;
	ss >> scratch;
	while( !ss.eof()){
		bool dSet = false;
		if(scratch == "Ns"){
			ss >> material.phong;
		}else if(scratch == "Ka"){
			ss >> kaR;
			ss >> kaG;
			ss >> kaB;
			material.Ka << kaR,   0, 0,
							0,  kaG, 0,
							0,    0, kaB;
		}else if(scratch == "Kd"){
			ss >> kdR;
			ss >> kdG;
			ss >> kdB;
			material.Kd << kdR,   0, 0,
							0,  kdG, 0,
							0,    0, kdB;
		}else if(scratch == "Ks"){
			ss >> ksR;
			ss >> ksG;
			ss >> ksB;
			material.Ks << ksR,   0, 0,
							0,  ksG, 0,
							0,    0, ksB;
		}else if(scratch == "d"){
			ss >> material.d;
			dSet = true;
		}
		ss >> scratch;
		if(dSet){
			matls.push_back(material);
			dSet=false;
		}
	}
	// for(unsigned int i=0; i< matls.size(); i++){
	// 	cout << "index " << i <<": order of vals: phong,Ka,Kd,Ks,d\n";
	// 	cout << matls[i].phong << '\n' << matls[i].Ka << '\n' << matls[i].Kd << '\n'
	// 	<< matls[i].Ks << '\n' << matls[i].d << '\n';
	// }
}

int Object3d::readObj()						// Read obj file
{
	int whichMatl = 0;//Which material is currently being used on faces (0 is 1st, 1 is 2nd...)
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
			vertexNorms.append(nextLine);
			vertexNorms.append("\n");
			numVertNorms++;
			nextLine = storeVertNorms(objInfo);
		}else if(isS(nextLine)){
			getline(objInfo,nextLine);
		}else if(isF(nextLine)){ 
			faces.append(nextLine);
			faces.append("\n");
			numFaces++;
			if(matlUseCount.empty()){
				matlUseCount.push_back(0);
			}
			matlUseCount[whichMatl]++;
			nextLine = storeFaces(objInfo,whichMatl);
		}else if(isMatl(nextLine)){
			storeMatl(nextLine);
			getline(objInfo,nextLine);
		}else if(isUseMatl(nextLine)){
			matlUseCount.push_back(0);
			if(matlUseCount.size() > 1){
				whichMatl++;
			}
			getline(objInfo,nextLine);
		}else{
			//isV, isF, isVn or isMatl may have run out of lines and returned nothing to nextLine
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

int Object3d::convertVertexList() // handles both vertices and vertex normals
{
	stringstream ssVert;
	char c;
	ssVert << vertexList;
	vertices.resize(4,numVertices);
	newVertices.resize(4,numVertices); // New points after transformation (handled in Model)
	//Handling vertices first
	for(int col=0; col<vertices.cols(); col++){
		ssVert >> c; // Eat up the 'v'
		for(int row=0; row<vertices.rows(); row++){
			if(!ssVert.eof()){
				if(row !=3){
					ssVert >> vertices(row,col);
				}else{
					vertices(row,col) = 1;
				}
			}
		}
	}
	//cout << "vertexMatrix:\n" << vertices << '\n';
	// Handling vertex normals next
	stringstream ssNorm;
	ssNorm << vertexNorms;
	norms.resize(4,numVertNorms);
	newNorms.resize(4,numVertNorms);   // New norms after transformation (handled in Model)
	for(int col=0; col<norms.cols(); col++){
		ssNorm >> c; // Eat up the 'v'
		ssNorm >> c; // Eat up the 'n'
		for(int row=0; row<norms.rows(); row++){
			if(!ssNorm.eof()){
				if(row !=3){
					ssNorm >> norms(row,col);
				}else{
					norms.col(col).normalize();
					norms(row,col) = 1;
				}
			}
		}
	}
	//cout << "vertNormals Matrix:\n" << norms << "\n";
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

bool Object3d::isMatl(const string& s)
{
	for(unsigned int i=0; i<s.length(); i++){
		if(s[i] == 'm'){
			if(s.substr(i,6) == "mtllib") return true;
		}
		if(!isspace(s[i])) return false;
	}
	return false;
}

bool Object3d::isUseMatl(const string& s)
{
	for(unsigned int i=0; i<s.length(); i++){
		if(s[i] == 'u'){
			if(s.substr(i,6) == "usemtl") return true;
		}
		if(!isspace(s[i])) return false;
	}
	return false;
}

int Object3d::writeObj(const string& dir)					// Write transformed obj file
{
	ofstream oFile;
	string temp = objName;
	int repeat=0;
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