// This is brl/bseg/bvxm/bvxm_von_mises_tangent_processor.h
#ifndef bvxm_von_mises_tangent_processor_h_
#define bvxm_von_mises_tangent_processor_h_
//:
// \file
// \brief A class for a 3-d tangent vector process
//
// \author Joseph L. Mundy
// \date Aug. 8, 2009
// \verbatim
//  Modifications
//   <none>
// \endverbatim

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_von_mises.h>
#include <vpgl/vpgl_proj_camera.h>

//:
// uses the VonMises distribution on the sphere of unit vectors
// and a 3-d spherical gaussian distribution for sub-voxel vector origin
// within the voxel
// The update process can be accelerated by a factor k, where
// mu^N+1 = 1-(k/N+1)mu^N + s*k/(N+1), and mu is the current
// distribution mean (direction or position) and N+1 is the current
// number of observations. If k==1 the update corresponds to simple
// averaging. It is necessary to have N > k-1 in order for the mean
// weight to be positive.
template <class T>
class  bvxm_von_mises_tangent_processor
{
 public:

  typedef bsta_vsum_num_obs<bsta_von_mises<T, 3> > dir_dist_t;
  typedef bsta_num_obs<bsta_gaussian_sphere<T, 2> > pos_dist_t;
  typedef typename bsta_von_mises<T, 3>::vector_type dir_t;
  typedef typename bsta_gaussian_sphere<T, 2>::vector_type pos_t;
  typedef T obs_math_t;

  bvxm_von_mises_tangent_processor() :
    theta_max_(static_cast<obs_math_t>(0.1)),
    x0_radius_(static_cast<obs_math_t>(0.2)), k_(T(1)) {}

  bvxm_von_mises_tangent_processor(T theta_max, T x0_radius, T k) :
    theta_max_(theta_max), x0_radius_(x0_radius), k_(k) {}

  virtual ~bvxm_von_mises_tangent_processor() = default;

  bool update( bvxm_voxel_slab<dir_dist_t> & dir_dist,
               bvxm_voxel_slab<pos_dist_t> & pos_dist,
               bvxm_voxel_slab<dir_t> const& dir,
               bvxm_voxel_slab<pos_t> const& pos,
               bvxm_voxel_slab<bool> const& flag);

  //: A helpful utility function to map two 2-d image tangents to a 3-d line
  static bool tangent_3d_from_2d(T img_a0, T img_b0, T img_c0,
                                 vpgl_proj_camera<double> const& cam0,
                                 T img_a1, T img_b1, T img_c1,
                                 vpgl_proj_camera<double> const& cam1,
                                 vgl_infinite_line_3d<T>& line_3d);

  static bool pos_dir_from_tangent_plane(vgl_plane_3d<T> const& plane,
                                         pos_dist_t const& pos_dist,
                                         dir_dist_t const& dir_dist,
                                         vgl_infinite_line_3d<T>& line_3d);

  static bool pos_dir_from_image_tangent(T img_a, T img_b, T img_c,
                                         vpgl_proj_camera<double> const& cam,
                                         pos_dist_t const& pos_dist,
                                         dir_dist_t const& dir_dist,
                                         vgl_infinite_line_3d<T>& line_3d);

 private:
  T theta_max_;
  T x0_radius_;
  T k_;
};

#define BVXM_VON_MISES_TANGENT_PROCESSOR_INSTANTIATE(T) extern "please include bvxm/bvxm_von_mises_tangent_processor.txx first"

#endif // bvxm_von_mises_tangent_processor_h_
