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
  bugl_uncertainty_point_2d() : vgl_point_2d<T>(){exists_ = false;}
  bugl_uncertainty_point_2d(T x, T y) : vgl_point_2d<T>(x,y){exists_ = true;}
  bugl_uncertainty_point_2d(vgl_point_2d<T> &p) : vgl_point_2d<T>(p){exists_ = true;}
 ~bugl_uncertainty_point_2d(){}
 void set_existence(bool exists){exists_ = exists;}
 bool exists(){return exists_;}
 protected:
 bool exists_;//does the point exist
#if 0
  virtual double probability(vgl_point_2d<T> p)=0;
#endif
};

#endif
