//#=====================================================================================
//#
//#       Filename:  camera.h
//#
//#    Description:  
//#
//#        Version:  1.0
//#        Created:  03/16/2003
//#       Revision:  none
//#       Compiler:  MSVC
//#
//#         Author:  Kongbin Kang  (kk)
//#        Company:  Brown University
//#          Email:  kk@lems.brown.edu
//#
//#=====================================================================================

#include <vnl/vnl_double_3x4.h>
#include<vcl_vector.h>

#if !defined __CAMERA_H__
#define __CAMERA_H__

// a structure used to store the lens distortion parameters
// It provide facility for set it on and off.
// the difination of each bits is given at Brown's paper.

class LensModel{
	double _kc[7];
	bool _bFlags[7]; // to show which distortion is used
public:
	LensModel() { 
		for (int i=0; i<7; i++){
			_kc[i] = 0;
			_bFlags[i] = false;
		}
	}

	double& operator[](int i) {return _kc[i];}
	bool isValid(int i) { return _bFlags[i];}
	inline void turnOn(int i) { _bFlags[i] = true;}
	inline void turnOff(int i) { _bFlags[i] = false;}
};


// an abstract camera definition. 
class Camera{
private:
	vnl_double_3x4 _k;
	LensModel _lm;
public:
	vnl_double_3x4 getIntrisicMatrix(){ return _k;}
	void setLensModel(vcl_vector<bool> flags);
	
	int setIntrisicMatrix(vnl_double_3x4 k) { 
		assert(k.rows()==3 && k.cols()==4);
		_k = k;
	}
	
	Camera();
	~Camera();
};

#endif
