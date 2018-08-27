// Bradley Pospeck
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
}