// This is core/vgl/algo/vgl_fit_plane_3d.h
#ifndef vgl_fit_plane_3d_h_
#define vgl_fit_plane_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Fits a plane to a set of 3D points
// \author Gamze D. Tunali
// \date December 18, 2006
//
//
//  By using the plane formula ax+by+cz+d=0, linear equations are derived
//  and solved for the paramaters (a, b, c, d)
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vcl_vector.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

template <class T>
class vgl_fit_plane_3d
{
  // Data Members--------------------------------------------------------------
 protected:
  vcl_vector<vgl_homg_point_3d<T> > points_;
  vgl_homg_plane_3d<double> plane_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

   vgl_fit_plane_3d() {}

   vgl_fit_plane_3d(vcl_vector<vgl_homg_point_3d<T> > points);

  ~vgl_fit_plane_3d() {}

  // Operations---------------------------------------------------------------

  //: add a point to point set
  void add_point(vgl_homg_point_3d<T> const &p);
  void add_point(const T x, const T y, const T z);

  //: clear internal data
  void clear();

  //:fits a plane to the stored points
  void fit();

  // Data Access---------------------------------------------------------------

  vcl_vector<vgl_homg_point_3d<T> >& get_points(){return points_;}

  //: first fit() should be called to get the plane computed
  vgl_homg_plane_3d<T>& get_plane() {return plane_;}
};

#define VGL_FIT_PLANE_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_plane_3d.txx first"

#endif // vgl_fit_plane_3d_h_
