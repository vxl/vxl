#ifndef BUGL_UNCERTAINTY_POINT_3D_H_
#define BUGL_UNCERTAINTY_POINT_3D_H_
//:
// \file
// \author Kongbin Kang
// \brief An abstract class for 3d point with uncertainty / probability distribution

#include <vgl/vgl_point_3d.h>

template<class T>
class bugl_uncertainty_point_3d : public vgl_point_3d<T>
{
 public:
  bugl_uncertainty_point_3d() : vgl_point_3d<T>() { exists_=false; }
  bugl_uncertainty_point_3d(T x, T y, T z) : vgl_point_3d<T>(x,y,z) { exists_=true; }
  bugl_uncertainty_point_3d(vgl_point_3d<T> &p) : vgl_point_3d<T>(p) { exists_=true; }
 ~bugl_uncertainty_point_3d() {}
  void set_existence(bool e) { exists_ = e; }
  bool exists() { return exists_; }
 protected:
  bool exists_;//does the point exist
#if 0
  virtual double probability(vgl_point_3d<T> p)=0;
#endif
};

#endif
