// This is core/vpgl/vpgl_generic_camera.h
#ifndef vpgl_generic_camera_h_
#define vpgl_generic_camera_h_
//:
// \file
// \brief The generic camera
// \author J.L. Mundy
// \date April 10, 2011
//
//   A generic form of camera projection is defined by a set of 3-d rays
//   and a mapping between rays and image coordinates. This model has been
//   used to unify various camera types such as fisheye, cadadioptric, etc.
//   Although it is possible to encounter cameras where the rays are
//   general curves, this class restricts the geometry of a ray to a 3-d line.
//   In the generic camera it is not necessary that all the rays intersect
//   at a single point as in the projective camera. In this implementation,
//   the image-to-ray map is restricted to a 2-d array, even though in general
//   the map can be more complex. It is assumed that the ray field is smooth
//   so that local derivatives are defined
//
//   Pixels (point samples, really) are centered at integer values; consequently,
//   the leading edge of pixel (0,0) is technically (-0.5, -0.5).

// \verbatim
//  Modifications <none>
// \endverbatim

#include <iosfwd>
#include <string>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vpgl_generic_camera : public vpgl_camera<T>
{
 public:

  vpgl_generic_camera();
  vpgl_generic_camera( vbl_array_2d<vgl_ray_3d<T> > const& rays);
  vpgl_generic_camera( std::vector<vbl_array_2d<vgl_ray_3d<T> > > const& rays,
                                              std::vector<int> nrs,   std::vector<int> ncs  );
  ~vpgl_generic_camera() override = default;

  std::string type_name() const override { return "vpgl_generic_camera"; }

  //: The generic camera interface. u represents image column, v image row. Finds projection using a pyramid search over the rays and so not particularly efficient.
  void project(const T x, const T y, const T z, T& u, T& v) const override;

  //: the number of columns (u coordinate) in the ray image
  unsigned cols(int level) const {return rays_[level].cols();}
  unsigned cols() const { return rays_[0].cols();}

  //: the number of rows (v coordinate) in the ray image
  unsigned rows(int level) const {return rays_[level].rows();}
  unsigned rows() const { return rays_[0].rows();}

  //: the number of pyramid levels
  unsigned n_levels() {return static_cast<unsigned>(n_levels_);}

  //: the ray corresponding to a given pixel
  vgl_ray_3d<T> ray(const T u, const T v) const;

  //: a ray passing through a given 3-d point
  vgl_ray_3d<T> ray(vgl_point_3d<T> const& p) const;

  //: the ray index at a given level
  vbl_array_2d<vgl_ray_3d<T> >& rays(int level) { return rays_[level];}

  //: the nearest ray origin to the coordinate origin
  vgl_point_3d<T> min_ray_origin() {return min_ray_origin_;}
  vgl_vector_3d<T> min_ray_direction() {return min_ray_direction_;}

  //: the furthest ray origin from the coordinate origin
  vgl_point_3d<T> max_ray_origin() {return max_ray_origin_;}
  vgl_vector_3d<T> max_ray_direction() {return max_ray_direction_;}

  //: debug function
  void print_orig(int level);

  //: visualization
  void print_to_vrml(int level, std::ostream& os);

 protected:
  void nearest_ray_to_point(vgl_point_3d<T> const& p,
                            int& nearest_r, int& nearest_c) const;
  //: nearest ray at level
  void nearest_ray(int level, vgl_point_3d<T> const& p,
                   int start_r, int end_r, int start_c, int end_c,
                   int& nearest_r, int& nearest_c) const;

  //: refine the projection to sub pixel
  void refine_projection(int nearest_c, int nearest_r,
                         vgl_point_3d<T> const& p, T& u, T& v) const;

  //: refine ray
  void refine_ray_at_point(int nearest_c, int nearest_r,
                           vgl_point_3d<T> const& p,
                           vgl_ray_3d<T>& ray) const;

  // === members ===

  //: ray origin bound to support occlusion reasoning
  vgl_point_3d<T> min_ray_origin_;
  vgl_vector_3d<T> min_ray_direction_;
  //: ray origin bound to support occlusion reasoning
  vgl_point_3d<T> max_ray_origin_;
  vgl_vector_3d<T> max_ray_direction_;

  //: a pyramid data structure for the rays to support efficient projection
  // (level == 0 is the highest resolution)
  unsigned long n_levels_;
  //: num rows at each resolution level
  std::vector<int> nr_;
  //: num cols at each resolution level
  std::vector<int> nc_;
  //: the pyramid
  std::vector<vbl_array_2d<vgl_ray_3d<T> > > rays_;
};

#endif // vpgl_generic_camera_h_
