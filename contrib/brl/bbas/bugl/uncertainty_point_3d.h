#ifndef UNCERTAINTY_POINT_3D_H_
#define UNCERTAINTY_POINT_3D_H_
//:
// \file
// \author Kongbin Kang
// An abstract class for 2d point with uncertainty / probabiblty distribution
// 
// \verbatim
// \endverbatim

#include <vgl/vgl_point_2d.h>

template<class T>
class bugl_uncertainty_point_2d : public vgl_point_2d<T>
{
public:
  bugl_uncertainty_point_2d(T x, T y) : vgl_point_2d<T>(x,y){}
  ~bugl_uncertainty_point_2d(){}

public:

#if 0
  virtual double get_probabilty(vgl_point_2d<T> p)=0;
#endif

};

#endif