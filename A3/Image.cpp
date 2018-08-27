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

void Image::calcSphCol(Sphere& sphere,const Eigen::Vector3d& ray,const double& t, const int& index)
{
	Vector3d ptos = pixels[index] + t*ray;	// ptos=point on sphere here
	Vector3d sphCenter(sphere.cx,sphere.cy,sphere.cz);
	Vector3d surfNorm = ptos - sphCenter;
	surfNorm.normalize();
	// get ambient lighting
	rgbPix[index](0) = static_cast<int>(ambR * sphere.kaR * 255);
	rgbPix[index](1) = static_cast<int>(ambG * sphere.kaG * 255);
	rgbPix[index](2) = static_cast<int>(ambB * sphere.kaB * 255);
	//cout << rgbPix[index] << '\n';
	// Iterate through each light to get diffuse and spectral lighting on the sphere
	Vector3d toL; // vector from sphere to light
	Vector3d toC; // vector from sphere to camera
	Vector3d R;   // Reflection vector
	for(int i=0; i<numLights; i++){
		toL = lightCollect[i].getLoc() - ptos;
		toL.normalize();
		if(toL.dot(surfNorm) > 0){ // The surface normal is facing the light
			Vector3d color = 255 * (sphere.getKd()*lightCollect[i].getB() * toL.dot(surfNorm));
			//cout << color << '\n';
			//cout << sphere.getKd() << "\n\n";
			rgbPix[index] += color.cast<int>(); // Cast rgb vals to integers
			toC = eye - ptos;
			toC.normalize();
			R = (2*toL.dot(surfNorm)*surfNorm) - toL;
			if(toC.dot(R) > 0){ // Light is reflecting to camera, not away
				color = 255*(sphere.getKs()*lightCollect[i].getB() * pow(toC.dot(R),sphere.phong));
				rgbPix[index] += color.cast<int>();
			}
			
		}
	}
	//cout << rgbPix[index] << '\n';
}

void Image::calcObjCol(Object3d& object,const Eigen::Vector3d& ray,const double& t, const int& index, const int& faceInd)
{
	// get ambient lighting
	Vector3d color;
	Vector3d ambient(ambR,ambG,ambB);
	color = object.ka * ambient * 255;
	rgbPix[index] = color.cast<int>();
	// Prepare for diffuse and specular
	Vector3d ptos = pixels[index] + t*ray; // ptos = point on surface here
	//Vector3d surfNorm = (object.Na[faceInd] + object.Nb[faceInd] + object.Nc[faceInd])/3;
	Vector3d surfNorm =(object.A[faceInd]-object.B[faceInd]).cross(object.C[faceInd]-object.A[faceInd]);
	surfNorm.normalize();
	//cout << "surfNorm:\n" << surfNorm << '\n';
	Vector3d toL; // vector from surface to light
	Vector3d toC; // vector from surface to camera
	Vector3d R;	  // Reflection vector
	for(int i=0; i<numLights; i++){
		toL = lightCollect[i].getLoc() - ptos;
		toL.normalize();
		//cout << "toL:\n" << toL << '\n';
		if(toL.dot(surfNorm) > 0){ // surface normal is facing the light
			color = 255 * (object.kd * lightCollect[i].getB() * toL.dot(surfNorm));
			//cout << object.kd << '\n';
			rgbPix[index] += color.cast<int>(); // Cast diffuse light to integers
			toC = eye - ptos;
			toC.normalize();
			R = (2*toL.dot(surfNorm)*surfNorm) - toL;
			if(toC.dot(R) > 0){
				color = 255*(object.ks * lightCollect[i].getB() * pow(toC.dot(R),object.getPhong()));
				rgbPix[index] += color.cast<int>();
			}
		}
	}
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

void Image::findIntersection() //Currently without sphere intersection considered
{
	double ax,ay,az,bx,by,bz,cx,cy,cz,lx,ly,lz,dx,dy,dz;
	double detM,detM1,detM2,detM3;
	double t;
	Vector3d vect;
	int index;
	tVals.resize(resWidth*resHeight,100000.0);
	Vector3i defaultCol(0,0,0);
	rgbPix.resize(resWidth*resHeight,defaultCol);
	for(int j=0; j<resWidth; j++){ // Pixel column
		for(int i=0; i<resHeight; i++){ // Pixel row
			index = j*resHeight+i;
			lx = pixels[index](0);
			ly = pixels[index](1);
			lz = pixels[index](2);
			dx = rays[index](0);
			dy = rays[index](1);
			dz = rays[index](2);
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

					// M << ax-bx, ax-cx, dx,
					// 	 ay-by, ay-cy, dy,
					// 	 az-bz, az-cz, dz;
					
					detM = ((az-cz)*dy -(ay-cy)*dz)*(ax-bx) - ((az-cz)*dx - (ax-cx)*dz)*(ay-by) + ((ay-cy)*dx - (ax-cx)*dy)*(az-bz);
					//detM = M.determinant();
					if(abs(detM) < 0.0000001){
						continue;
					}
					// M1 << ax-lx, ax-cx, dx,
					// 	  ay-ly, ay-cy, dy,
					// 	  az-lz, az-cz, dz;
					detM1 = ((az-cz)*dy -(ay-cy)*dz)*(ax-lx) - ((az-cz)*dx - (ax-cx)*dz)*(ay-ly) + ((ay-cy)*dx - (ax-cx)*dy)*(az-lz);
					//detM1 = M1.determinant();
					beta = detM1/detM;
					if(beta <0) continue; // Beta isn't 0+ so skip further calculation
					// M2 << ax-bx, ax-lx, dx,
					// 	  ay-by, ay-ly, dy,
					// 	  az-bz, az-lz, dz;
					detM2 = ((az-lz)*dy -(ay-ly)*dz)*(ax-bx) - ((az-lz)*dx - (ax-lx)*dz)*(ay-by) + ((ay-ly)*dx - (ax-lx)*dy)*(az-bz);
					//detM2 = M2.determinant();
					gamma = detM2/detM;
					if(gamma <0 || (beta+gamma) >1.000001) continue;//Gamma isn't 0+ and/or beta+gamma isn't 1 or less, so skip further calc
					// M3 << ax-bx, ax-cx, ax-lx,
					// 	  ay-by, ay-cy, ay-ly,
					// 	  az-bz, az-cz, az-lz;
					detM3 = ((ay-ly)*(az-cz) - (ay-cy)*(az-lz))*(ax-bx) - ((ax-lx)*(az-cz) - (ax-cx)*(az-lz))*(ay-by) + ((ax-lx)*(ay-cy) - (ax-cx)*(ay-ly))*(az-bz);
					//detM3 = M3.determinant();
					t = detM3/detM;
					if(t>0){ // t must be a positive distance, or it isn't in camera's view
						if(t < tVals[index]){ // Only want to store value for object closest to camera
							tVals[index] = t;
							calcObjCol(mods->collection[m].obj,rays[index],t,index,f);
						}
					}
				}
			}
			for(int s=0; s<numSpheres; s++){
				sphereCollect[s].setC(pixels[index]);
				sphereCollect[s].setV(rays[index]);
				t = sphereCollect[s].rayIntersection();
				if(t>0 && t < tVals[index]){
					tVals[index] = t;
					calcSphCol(sphereCollect[s],rays[index],t,index); //calculate sphere color
				}
			}
		}
	}
	for(int j=0; j<resWidth; j++){
		for(int i=0; i<resHeight; i++){
			t = tVals[j*resHeight+i];
			if(t < tmin){
				tmin=t;
			}
			if(t > tmax && t != 100000){
				tmax=t;
			}
		}
	}
	cout << "(tmin,tmax): (" << tmin << "," << tmax << ")\n";
}

void Image::setColor()
{
	double ratio,t;
	Eigen::Vector3i color;
	for(int j=0; j<resWidth; j++){
		for(int i=0; i<resHeight; i++){
			t = tVals[j*resHeight+i];
			if(t != 100000){
				ratio = 2* (t - tmin) / (tmax-tmin);
				color(0) = max(0, static_cast<int>(255*(1-ratio))); //red
				color(2) = max(0, static_cast<int>(255*(ratio-1))); //blue
				color(1) = 255 - color(2) - color(0); //green
			}else{
				color << 239,239,239;
			}
			rgbPix.push_back(color);
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