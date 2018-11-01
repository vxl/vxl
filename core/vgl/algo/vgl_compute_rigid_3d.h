// This is core/vgl/algo/vgl_compute_rigid_3d.h
#ifndef vgl_compute_rigid_3d_h_
#define vgl_compute_rigid_3d_h_
//:
// \file
// \brief Compute a rigid transformation between two corresponding sets of 3D points
// \author Matt Leotta, Dan Crispell
// \date April 15 2016
//
//
//  Estimate translation \a t and rotation \a R such that
//  sum ||R*p1 + t - p2||  is minimized over all pairs (p1,p2)
//
// \verbatim
//  Modifications
//    dec: Adapted from vgl_compute_similarity_3d
// \endverbatim

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>


template <class T>
class vgl_compute_rigid_3d
{
 public:

  // Constructors/Initializers/Destructors-------------------------------------

   vgl_compute_rigid_3d() = default;

   vgl_compute_rigid_3d(std::vector<vgl_point_3d<T> > const& points1,
                             std::vector<vgl_point_3d<T> > const& points2);

  ~vgl_compute_rigid_3d() = default;

  // Operations---------------------------------------------------------------

  //: add a pair of points to point sets
  void add_points(vgl_point_3d<T> const &p1,
                  vgl_point_3d<T> const &p2);

  //: clear internal data
  void clear();

  //: estimates the rigid transformation from the stored points
  bool estimate();

  // Data Access---------------------------------------------------------------

  //: Access the estimated rotation
  const vgl_rotation_3d<T>& rotation() const { return rotation_; }

  //: Access the estimated translation
  const vgl_vector_3d<T>& translation() const { return translation_; }

 protected:
  // Internal functions--------------------------------------------------------

  //: center all the points at the origin, and return the applied translation
  void center_points(std::vector<vgl_point_3d<T> >& pts,
                     vgl_vector_3d<T>& t) const;

  //: normalize the scale of the points, and return the applied scale
  //  The average distance from the origin will be sqrt(3)
  void scale_points(std::vector<vgl_point_3d<T> >& pts,
                    T& s) const;

  // Data Members--------------------------------------------------------------
  std::vector<vgl_point_3d<T> > points1_;
  std::vector<vgl_point_3d<T> > points2_;
  vgl_rotation_3d<T> rotation_;
  vgl_vector_3d<T> translation_;
};

#define VGL_COMPUTE_RIGID_3D_INSTANTIATE(T) \
extern "please include vgl/algo/vgl_compute_rigid_3d.hxx first"

#endif // vgl_compute_rigid_3d_h_
