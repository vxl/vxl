#ifndef BUGL_UNCERTAINTY_POINT_3D_H_
#define BUGL_UNCERTAINTY_POINT_3D_H_
//:
// \file
// \author Kongbin Kang
// An abstract class for 3d point with uncertainty / probabiblty distribution
// 
// \verbatim
// \endverbatim

#include <vgl/vgl_point_3d.h>

template<class T>
class bugl_uncertainty_point_3d : public vgl_point_3d<T>
{
public:
  bugl_uncertainty_point_3d(T x, T y, T z) : vgl_point_3d<T>(x,y,z){}
  ~bugl_uncertainty_point_3d(){}

public:

#if 0
  virtual double get_probabilty(vgl_point_3d<T> p)=0;
#endif

};

#endif
