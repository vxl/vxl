#ifndef UNCERTAINTY_POINT_2D_H_
#define UNCERTAINTY_POINT_2D_H_
//:
// \file
// \author Kongbin Kang
// \brief An abstract class for 2d point with uncertainty / probability distribution

#include <vgl/vgl_point_2d.h>

template<class T>
class bugl_uncertainty_point_2d : public vgl_point_2d<T>
{
 public:
  bugl_uncertainty_point_2d() : vgl_point_2d<T>(){}
  bugl_uncertainty_point_2d(T x, T y) : vgl_point_2d<T>(x,y){}
  bugl_uncertainty_point_2d(vgl_point_2d<T> &p) : vgl_point_2d<T>(p){}
 ~bugl_uncertainty_point_2d(){}

#if 0
  virtual double probability(vgl_point_2d<T> p)=0;
#endif
};

#endif
