#ifndef BUGL_UNCERTAINTY_POINT_3D_H_
#define BUGL_UNCERTAINTY_POINT_3D_H_
//:
// \file
// \author Kongbin Kang
// \brief An abstract class for 3d point with uncertainty / probability distribution
//
// \verbatim
//  Modifications
//   10 Sept 2004 Peter Vanroose - made prob_at() pure virtual
//   10 Sept 2004 Peter Vanroose - access to member exists_ restricted (consistency!)
// \endverbatim

#include <vgl/vgl_point_3d.h>

template<class T>
class bugl_uncertainty_point_3d : public vgl_point_3d<T>
{
 public:
  bugl_uncertainty_point_3d() : vgl_point_3d<T>(), exists_(false) {}
  bugl_uncertainty_point_3d(T x, T y, T z) : vgl_point_3d<T>(x,y,z), exists_(true) {}
  bugl_uncertainty_point_3d(vgl_point_3d<T> const& p) : vgl_point_3d<T>(p), exists_(true) {}
  virtual ~bugl_uncertainty_point_3d() {}
  virtual T prob_at(vgl_point_3d<T> const& p) const = 0;
  bool exists() const { return exists_; }
  //: set as if this point is not initialised (as with default constructor)
  void set_point() { exists_ = false; }
  void set_point(vgl_point_3d<T> const& p) { exists_=true; this->set(p.x(), p.y(), p.z()); }
 protected:
  bool exists_;//does the point exist
};

#endif
