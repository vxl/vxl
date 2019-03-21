// This is brl/bseg/bsgm/bsgm_align_pointsets_3d.h
#ifndef bsgm_align_pointsets_3d_h_
#define bsgm_align_pointsets_3d_h_

#include <iostream>
#include <vector>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>

template <typename T>
struct alignment_params
{
  alignment_params():point_sample_dist_(T(1)/T(2)), coarse_inc_mul_(T(10)), fine_inc_mul_(T(5)/T(2)),
    coarse_radius_mul_(T(40)), fine_radius_mul_(coarse_inc_mul_*T(5)/T(4)), consen_dist_mul_(T(2)),
    min_n_pts_(2500) {}

  T point_sample_dist_; //sets the spatial resolution for the search space
  T coarse_inc_mul_;    //the translation vector increment for coarse search
  T fine_inc_mul_;      //the translation vector increment for fine search
  T coarse_radius_mul_; //the coarse search radius
  T fine_radius_mul_;   //the fine search radius

  T consen_dist_mul_;   //the max distance a transformed point from pointset 0
                        //can be from the nearest point in the pointset 1 (2)
  size_t min_n_pts_;    // the minium number of points in pointset 0 to be tested for consensus
                        // with respect to pointset 1
};

template <typename T>
class bsgm_align_pointsets_3d
{
 public:
  bsgm_align_pointsets_3d(vgl_pointset_3d<T> const& ptset_0, vgl_pointset_3d<T> const& ptset_1):
    ptset_0_(ptset_0), ptset_1_(ptset_1)
  {
    knn_ = bvgl_k_nearest_neighbors_3d<T>(ptset_1);
  }

  //: the function for computing the consensus for a transformation of pointset 1
  // the value is the number of nearest points in pointset 1 within c_tol of each
  // transformed point from pointset 0.
  size_t consensus(T tx, T ty, T tz, T c_tol);

  //: a loop that scans the search space looking for the maximum consensus
  // rad, inc the radius and increment for the search
  // tx0, ty0, tz0 - the center of the search space
  // con_tol the distance tolerance for consensus
  // max_tx, max_ty, max_tz the result of the search for maximum consensus, max_con
  bool search(T rad, T inc, T tx0, T ty0, T tz0, T con_tol,
              T& max_tx, T& max_ty, T& max_tz, size_t& max_con, bool verbose = false);

  //: the main process method. If verbose is true a series of array showing the search are printed
  bool find_translation(bool verbose = false);

  vgl_vector_3d<T> translation() const {return translation_;}

 private:
  alignment_params<T> params_;
  vgl_pointset_3d<T> ptset_0_;
  vgl_pointset_3d<T> ptset_1_;
  bvgl_k_nearest_neighbors_3d<T> knn_;
  vgl_vector_3d<T> translation_;
};


#endif // bsgm_align_pointsets_3d_h_
