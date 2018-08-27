// Bradley Pospeck
// Image class implementation

#include <Image.h>

Image::~Image()
{
	delete mods;
	delete[] sphereCollect;
	delete[] lightCollect;
}

bool Image::isComment(const string& s)
{
	for (unsigned int i = 0; i < s.length(); i++) {    
        if (s[i] == '#') return true;	
        if ( ! isspace(s[i]) ) return false;
    }
    return false;
}

void Image::readDriver(ifstream& sceneInfo)
{
	string line; 	 // Will be line by line parsing of driver file
	string scratch;
	double num;
	stringstream ss; // Will be used to check each line value by value
	if(sceneInfo.is_open()){
		while(getline(sceneInfo,line)){
			if(isComment(line)) continue;
			ss << line;
			//cout << line << '\n';
			while( !ss.eof()){
				if( ss >> scratch){
					if(scratch == "eye"){
						ss >> eye(0);
						ss >> eye(1);
						ss >> eye(2);
						//cout << "eye" << eye << '\n';
					}else if(scratch == "look"){
						ss >> look(0);
						ss >> look(1);
						ss >> look(2);
						//cout << "look" << look << '\n';
					}else if(scratch == "up"){
						ss >> up(0);
						ss >> up(1);
						ss >> up(2);
						//cout << "up" << up << '\n';
					}else if(scratch == "d"){
						ss >> focalLength;
						focalLength = -1*focalLength;
						//cout << "focalLength " << focalLength << '\n';
					}else if(scratch == "bounds"){
						ss >> left;
						ss >> bottom;
						ss >> right;
						ss >> top;
						//cout<<"(lf,bt,rt,tp)"<<"("<<left<<","<<bottom<<","<<right<<","<<top<<")\n";
					}else if(scratch == "res"){
						ss >> resWidth;
						ss >> resHeight;
						//cout << "resolution " << resWidth << " " << resHeight << "\n";
					}else if(scratch == "recursionLevel"){
						ss >> recursion;
					}else if(scratch == "ambient"){
						ss >> ambR;
						ss >> ambG;
						ss >> ambB;
						//cout << "ambient light: \n";
						//cout << ambR << ' ' << ambG << ' ' << ambB << '\n';
					}else if(scratch == "light"){
						numLights++;
						int count = 7;
						while(count !=0){
							ss >> num;
							lights.append(std::to_string(num));
							lights.append(" ");
							count--;
						}
					}else if(scratch == "sphere"){
						numSpheres++;
						int count = 16;
						while(count !=0){
							ss >> num;
							spheres.append(std::to_string(num));
							spheres.append(" ");
							count--;
						}
					}else if(scratch == "model"){ 
						numModels++;
						int count = 9;
						while(count !=0){
							if(count !=1){
								ss >> num;
								models.append(std::to_string(num));
							}else{
								ss >> scratch;
								models.append(scratch);
							}	
							models.append(" ");						
							count--;
						}
					}
				}
			}
			ss.str("");
			ss.clear();
		}
		//cout << spheres << "\n";
		//cout << models << '\n';
		//cout << "numModels initially: " << numModels << '\n';
		sceneInfo.close();
	}else{
		cerr << "Error opening file\n";
	}
}

void Image::setCamera()
{
	// W = eye-look/magnitude(eye-look)
	camW = eye-look;
	camW.normalize();
	// U = up x W/ magnitude(up x W)
	camU = up.cross(camW);
	camU.normalize();
	// V = W x U
	camV = camW.cross(camU);
	//cout << "camU " << camU << '\n';
	//cout << "camV " << camV << '\n';
	//cout << "camW " << camW << '\n';
}

void Image::setSpheres()
{
	//cout << "inside setSpheres..\n";
	sphereCollect = new Sphere[numSpheres];
	double val;
	int index =0;
	stringstream ss;
	ss << spheres;
	while ( !ss.eof()){
		if( !(ss >> val)){
			break;
		}
		sphereCollect[index].cx = val;
		ss >> sphereCollect[index].cy;
		ss >> sphereCollect[index].cz;
		ss >> sphereCollect[index].radius;
		ss >> sphereCollect[index].kaR;
		ss >> sphereCollect[index].kaG;
		ss >> sphereCollect[index].kaB;
		ss >> sphereCollect[index].kdR;
		ss >> sphereCollect[index].kdG;
		ss >> sphereCollect[index].kdB;
		ss >> sphereCollect[index].ksR;
		ss >> sphereCollect[index].ksG;
		ss >> sphereCollect[index].ksB;
		ss >> sphereCollect[index].krR;
		ss >> sphereCollect[index].krG;
		ss >> sphereCollect[index].krB;
		sphereCollect[index].setK();
		//sphereCollect[index].printSphere();
		index++;
	}
}

void Image::setLights()
{
	lightCollect = new Light[numLights];
	double val;
	int index=0;
	stringstream ss;
	ss << lights;
	while( !ss.eof()){
		if( !(ss >> val)){
			break;
		}
		lightCollect[index].x = val;
		ss >> lightCollect[index].y;
		ss >> lightCollect[index].z;
		ss >> lightCollect[index].w;
		ss >> lightCollect[index].red;
		ss >> lightCollect[index].green;
		ss >> lightCollect[index].blue;
		lightCollect[index].setLight();
		//lightCollect[index].printLight();
		index++;
	}
}

void Image::setModels(const string& dName)
{
	mods = new Models(numModels); // Class that handles multiple models
	mods->readTransforms(models);
	//cout << "after readTransforms...\n";
	mods->setAxisAngleRot();
	//cout << "after setAxisAngleRot...\n";
	mods->transform(dName);	// Handles transformation and file output
	//cout << "Finished everything\n";
	// delete here for now, may move later if necessary
}

double Image::getPx(const int& i)
{
	return i/(static_cast<double>(resWidth)-1) *(right - left) + left;
}

double Image::getPy(const int& j)
{
	return j/(static_cast<double>(resHeight)-1) *(top - bottom) + bottom;
}

void Image::setPixels()
{
	Vector3d pixPoint;
	double px;
	double py;
	// Pixel location= eye +n*camW + alpha*camU + beta*camV
	// n=focalLength, alpha=px, beta=py
	for(int h=0; h<resHeight; h++){
		py = getPy(resHeight-1-h);
		for(int w=0; w<resWidth; w++){
			//py = getPy(w);
			px = getPx(w);
			//py = getPy(resWidth-1-w);
			pixPoint = eye + focalLength*camW + px*camU + py*camV;
			pixels.push_back(pixPoint);
			//cout << "pixel at (" << w << "," << h << ") is: " << pixels[w*resHeight+h] << '\n';
		}
	}	
}
/* Currently unused function below*/
bool Image:: isShadowFromSphere(Vector3d lightLoc, Vector3d point, Sphere sphere)
{
	Vector3d toL = lightLoc - point;
	Vector3d lDist = toL;
	toL.normalize();
	sphere.setV(toL);
	double t = sphere.rayIntersection();
	if(t < lDist.dot(toL)){
		cout << "shadow...\n";
		return true;
	}else{
		return false;
	}
}

Vector3d Image::calcSphCol(Sphere& sphere,const Eigen::Vector3d& ray,const double t, Vector3d& point)
{
	Vector3d ptos = point + t*ray;	// ptos=point on sphere here
	Vector3d sphCenter(sphere.cx,sphere.cy,sphere.cz);
	Vector3d surfNorm = ptos - sphCenter;
	Vector3d color;
	surfNorm.normalize();
	// get ambient lighting
	color(0) = static_cast<int>(ambR * sphere.kaR);
	color(1) = static_cast<int>(ambG * sphere.kaG);
	color(2) = static_cast<int>(ambB * sphere.kaB);
	// Iterate through each light to get diffuse and spectral lighting on the sphere
	Vector3d toL; // vector from sphere to light
	Vector3d toC = point - ptos; // vector from sphere to camera
	toC.normalize();
	if(toC.dot(surfNorm) < 0){
		surfNorm = -1*surfNorm;
	}
	Vector3d R;   // Reflection vector
	for(int i=0; i<numLights; i++){
		toL = lightCollect[i].getLoc() - ptos;
		toL.normalize();
		if(toL.dot(surfNorm) > 0){ // The surface normal is facing the light and there's no shadow
			color += (sphere.getKd()*lightCollect[i].getB() * toL.dot(surfNorm));
			R = (2*toL.dot(surfNorm)*surfNorm) - toL;
			if(toC.dot(R) > 0){ // Light is reflecting to camera, not away
				color += (sphere.getKs()*lightCollect[i].getB() * pow(toC.dot(R),sphere.phong));
			}		
		}
	}
	return color;
}

Vector3d Image::calcObjCol(Object3d& object,const Eigen::Vector3d& ray,const double t, Vector3d& point, const int& faceInd)
{
	int pickMatl=0;
	int sum=0;
	for(unsigned int i=0; i<object.matls.size(); i++){
		if( (object.matlUseCount[i]-1+sum) >= faceInd){ //1 matl use is faceInd 0, so subtract 1
			pickMatl = i;
			break;
		}
		sum += object.matlUseCount[i];
	}
	// get ambient lighting
	Vector3d color;
	Vector3d ambient(ambR,ambG,ambB);
	color = object.matls[pickMatl].Ka * ambient;
	// Prepare for diffuse and specular
	Vector3d ptos = point + t*ray; // ptos = point on surface here
	Vector3d surfNorm =(object.A[faceInd]-object.B[faceInd]).cross(object.C[faceInd]-object.A[faceInd]);
	surfNorm.normalize();
	//cout << "surfNorm:\n" << surfNorm << '\n';
	Vector3d toL; // vector from surface to light
	Vector3d R;	  // Reflection vector
	Vector3d toC = point - ptos; // vector from surface to camera
	toC.normalize();
	if(toC.dot(surfNorm) < 0){
		surfNorm = -1*surfNorm;
	}
	for(int i=0; i<numLights; i++){
		toL = lightCollect[i].getLoc() - ptos;
		//Vector3d lDist = toL; // Used to check shadows
		toL.normalize();
		//cout << "toL:\n" << toL << '\n';
		if(toL.dot(surfNorm) > 0){ // surface normal is facing the light and no shadow
			color += (object.matls[pickMatl].Kd * lightCollect[i].getB() * toL.dot(surfNorm));
			//cout << object.kd << '\n';
			R = (2*toL.dot(surfNorm)*surfNorm) - toL;
			if(toC.dot(R) > 0){
				color += (object.matls[pickMatl].Ks * lightCollect[i].getB() * pow(toC.dot(R),object.matls[pickMatl].phong));
			}
		}
	}
	return color;
}

void Image::setRays()
{
	Vector3d rayDir;
	Vector3d point;
	for(int col=0; col<resWidth; col++){
		for(int row=0; row<resHeight; row++){
			point = pixels[col*resHeight+row];
			rayDir = point - eye;
			rayDir.normalize();
			//cout << "ray: " << rayDir(0) << " " << rayDir(1) << " " << rayDir(2) << '\n';
			rays.push_back(rayDir);
		}
	}
}

Vector3d Image::rayTrace(Vector3d& point,Vector3d& ray,Vector3d accum,Vector3d refatt,int depth){
	double ax,ay,az,bx,by,bz,cx,cy,cz,lx,ly,lz,dx,dy,dz;
	double detM,detM1,detM2,detM3;
	double t;
	Vector3d vect;
	Vector3d objCol(0.0,0.0,0.0); // temporarily stores accum from obj in case something else closer
	Vector3d sphCol(0.0,0.0,0.0); // temporarily stores accum from sphere in case other spheres closer
	bool sphereCloser = false;
	Vector3d objKr(1.0,1.0,1.0); // Hard coded as 1's for all objects in A4
	Vector3d surfNorm; // Used later for reflection ray in recursion
	Sphere sphere;
	// Reset tBest with each bounce
	double tBest = 100000.0;

	lx = point(0);
	ly = point(1);
	lz = point(2);
	dx = ray(0);
	dy = ray(1);
	dz = ray(2);
	for(int m=0; m<numModels; m++){ //m for model
		for(int f=0; f<mods->collection[m].obj.getNumFaces(); f++){ //f for faces
			vect = mods->collection[m].obj.A[f];
			ax = vect(0);
			ay = vect(1);
			az = vect(2);
			vect = mods->collection[m].obj.B[f];
			bx = vect(0);
			by = vect(1);
			bz = vect(2);
			vect = mods->collection[m].obj.C[f];
			cx = vect(0);
			cy = vect(1);
			cz = vect(2);
			//Using Kramer's rule for determinants to keep code faster
			detM = ((az-cz)*dy -(ay-cy)*dz)*(ax-bx) - ((az-cz)*dx - (ax-cx)*dz)*(ay-by) + ((ay-cy)*dx - (ax-cx)*dy)*(az-bz);
			if(abs(detM) < 0.0000001) continue;
			//M1 determinant
			detM1 = ((az-cz)*dy -(ay-cy)*dz)*(ax-lx) - ((az-cz)*dx - (ax-cx)*dz)*(ay-ly) + ((ay-cy)*dx - (ax-cx)*dy)*(az-lz);
			beta = detM1/detM;
			if(beta <0) continue; // Beta isn't 0+ so skip further calculation
			//M2 determinant
			detM2 = ((az-lz)*dy -(ay-ly)*dz)*(ax-bx) - ((az-lz)*dx - (ax-lx)*dz)*(ay-by) + ((ay-ly)*dx - (ax-lx)*dy)*(az-bz);
			gamma = detM2/detM;
			if(gamma <0 || (beta+gamma) >1.000001) continue;//Gamma isn't 0+ and/or beta+gamma isn't 1 or less, so skip further calc
			//M3 determinant
			detM3 = ((ay-ly)*(az-cz) - (ay-cy)*(az-lz))*(ax-bx) - ((ax-lx)*(az-cz) - (ax-cx)*(az-lz))*(ay-by) + ((ax-lx)*(ay-cy) - (ax-cx)*(ay-ly))*(az-bz);
			t = detM3/detM;
			if(t>0.00001){ // t must be a positive distance, or it isn't in camera's view
				if(t < tBest){ // Only want to store value for object closest to camera
					//cout << "Current nearest face: " << f << ' ';
					tBest = t;
					Object3d object = mods->collection[m].obj;
					objCol = calcObjCol(object,ray,tBest,point,f);
					surfNorm = (object.A[f]-object.B[f]).cross(object.C[f]-object.A[f]);
				}
			}
		}
	}
	for(int s=0; s<numSpheres; s++){
		sphereCollect[s].setC(point);
		sphereCollect[s].setV(ray);
		t = sphereCollect[s].rayIntersection();
		if(t>0.00001 && t < tBest){
			tBest = t;
			sphere = sphereCollect[s];
			sphCol = calcSphCol(sphere,ray,tBest,point); //calculate sphere color
			Vector3d sphCenter(sphere.cx,sphere.cy,sphere.cz);
			surfNorm = (point + tBest*ray) - sphCenter;
			sphereCloser = true;
		}
	}
	if(tBest == 100000.0) return accum; // No more intersections, return now
	// If sphere is closer add attenuated sphCol to accum. Otherwise add attenuated objCol
	if(sphereCloser){ 
		accum(0) += sphCol(0) * refatt(0);
		accum(1) += sphCol(1) * refatt(1);
		accum(2) += sphCol(2) * refatt(2);
	}else{
		accum(0) += objCol(0) * refatt(0);
		accum(1) += objCol(1) * refatt(1);
		accum(2) += objCol(2) * refatt(2);
	}
	// Handle recursion down here
	if(depth > 0){
		Vector3d toC = -1 * ray;
		surfNorm.normalize(); // No need to do this operation before I have my final surfNorm
		Vector3d refRay = (2 * surfNorm.dot(toC) * surfNorm) - toC;
		Vector3d newPoint = point + tBest*ray;
		if(sphereCloser){
			refatt(0) = refatt(0) * sphere.krR;
			refatt(1) = refatt(1) * sphere.krG;
			refatt(2) = refatt(2) * sphere.krB;
			accum = rayTrace(newPoint,refRay,accum,refatt,depth-1);
		}else{
			refatt(0) = refatt(0) * objKr(0);
			refatt(1) = refatt(1) * objKr(1);
			refatt(2) = refatt(2) * objKr(2);
			accum = rayTrace(newPoint,refRay,accum,refatt,depth-1);
		}
	}
	return accum;
}

void Image::findIntersection() //Currently without sphere intersection considered
{
	int index;
	//tVals.resize(resWidth*resHeight,100000.0);
	Vector3i defaultCol(0,0,0);
	rgbPix.resize(resWidth*resHeight,defaultCol);
	for(int j=0; j<resWidth; j++){ // Pixel column
		for(int i=0; i<resHeight; i++){ // Pixel row
			index = j*resHeight+i;
			Vector3d point = pixels[index];
			Vector3d ray = rays[index];
			Vector3d accum(0.0,0.0,0.0);
			Vector3d refatt(1.0,1.0,1.0);
			int depth = recursion;
			accum = 255*rayTrace(point,ray,accum,refatt,depth);
			rgbPix[index] = accum.cast<int>();
			if(rgbPix[index](0) > 255) rgbPix[index](0) = 255;
			if(rgbPix[index](1) > 255) rgbPix[index](1) = 255;
			if(rgbPix[index](2) > 255) rgbPix[index](2) = 255;
		}
	}
}

void Image::writePpm(const string& ppm)
{
	ofstream output;
	output.open(ppm);
	if(output){
		output << "P3\n";
		output << resWidth << " " << resHeight << " " << 255 << '\n';
	}
	Eigen::Vector3i color;
	for(int i=0; i<resHeight; i++){
		for(int j=0; j<resWidth; j++){
			color = rgbPix[i*resWidth+j];
			output << color(0) << " " << color(1) << " " << color(2) << " ";
		}
		output << '\n';
	}
	output.close();
}