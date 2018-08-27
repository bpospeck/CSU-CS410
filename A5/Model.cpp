// Bradley Pospeck
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
	obj.setMatlProps();
	obj.convertVertexList();
	return 0;
}

int Model::transformObj(const string& dir)
{
	obj.newVertices = T*S*R*obj.vertices;
	obj.newNorms = T*S*R*obj.norms;
	//cout << "transformed vertices:\n" << obj.newVertices << "\n";
	obj.setObjFaces();
	//obj.writeObj(dir);
	return 0;
}