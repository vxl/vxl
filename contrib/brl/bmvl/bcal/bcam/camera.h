#ifndef bcam_CAMERA_H__
#define bcam_CAMERA_H__
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
#include <vcl_vector.h>
#include <vcl_cassert.h>

// a structure used to store the lens distortion parameters
// It provide facility for set it on and off.
// the difination of each bits is given at Brown's paper.

class LensModel
{
  double kc_[7];
  bool bFlags_[7]; // to show which distortion is used
 public:
  LensModel() {
    for (int i=0; i<7; i++){
      kc_[i] = 0;
      bFlags_[i] = false;
    }
  }

  double& operator[](int i) {return kc_[i];}
  bool isValid(int i) { return bFlags_[i];}
  inline void turnOn(int i) { bFlags_[i] = true;}
  inline void turnOff(int i) { bFlags_[i] = false;}
};


// an abstract camera definition.
class Camera
{
 private:
  vnl_double_3x4 k_;
  LensModel lm_;
 public:
  vnl_double_3x4 getIntrisicMatrix(){ return k_;}
  void setLensModel(vcl_vector<bool> flags);

  int setIntrisicMatrix(vnl_double_3x4 k) {
    assert(k.rows()==3 && k.cols()==4);
    k_ = k;
  }

  Camera();
  ~Camera(){};
};

#endif // bcam_CAMERA_H__
