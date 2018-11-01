// This is core/vgl/algo/vgl_fit_plane_3d.h
#ifndef vgl_fit_plane_3d_h_
#define vgl_fit_plane_3d_h_
//:
// \file
// \brief Fits a plane to a set of 3D points
// \author Gamze D. Tunali
// \date December 18, 2006
//
//  By using the plane formula ax+by+cz+d=0, linear equations are derived
//  and solved for the parameters (a, b, c, d)
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vector>
#include <iosfwd>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vgl_fit_plane_3d
{
  // Data Members--------------------------------------------------------------
 protected:
  std::vector<vgl_homg_point_3d<T> > points_;
  vgl_homg_plane_3d<T> plane_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

   vgl_fit_plane_3d() = default;

   vgl_fit_plane_3d(std::vector<vgl_homg_point_3d<T> > points);

  ~vgl_fit_plane_3d() = default;

  // Operations---------------------------------------------------------------

  //: add a point to point set
  void add_point(vgl_homg_point_3d<T> const &p);
  void add_point(const T x, const T y, const T z);

  //: clear internal data
  void clear();

  //:fits a plane to the stored points
  // report issues over an ostream if declared
  bool fit(const T error_marg, std::ostream* outstream=nullptr);

  //:fits a plane returning the smallest singular value
  //:of the data scatter matrix decomposition, a measure
  //:of variance in the direction of the plane normal
  T fit(std::ostream* outstream=nullptr);

  // Data Access---------------------------------------------------------------

  std::vector<vgl_homg_point_3d<T> >& get_points(){return points_;}

  //: first fit() should be called to get the plane computed
  vgl_homg_plane_3d<T>& get_plane() {return plane_;}
};

#define VGL_FIT_PLANE_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_plane_3d.hxx first"

#endif // vgl_fit_plane_3d_h_
