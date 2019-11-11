// This is contrib/brl/bbas/bpgl/algo/bpgl_heightmap_from_disparity.hxx
#ifndef bpgl_heightmap_from_disparity_hxx_
#define bpgl_heightmap_from_disparity_hxx_

#include <algorithm>
#include <vnl/vnl_math.h>
#include <vgl/vgl_box_3d.h>

#include "bpgl_3d_from_disparity.h"
#include "bpgl_heightmap_from_disparity.h"
#include "bpgl_gridding.h"


// main convenience function - transform disparity to heightmap
template<class T, class CAM_T>
vil_image_view<T> bpgl_heightmap_from_disparity(
    CAM_T const& cam1,
    CAM_T const& cam2,
    vil_image_view<T> const& disparity,
    vgl_box_3d<T> heightmap_bounds,
    T ground_sample_distance)
{
  // triangulated image
  vil_image_view<T> tri_3d = bpgl_3d_from_disparity(cam1, cam2, disparity);

  // convert triangulated image to heightmap
  bpgl_heightmap<T> bh(heightmap_bounds, ground_sample_distance);
  vil_image_view<T> heightmap_output;
  bh.heightmap_from_tri(tri_3d, heightmap_output);

  // cleanup
  return heightmap_output;
}


// ----------
// 3D pointset from triangulated input
// compute 3D pointset using disparity to triangulate 3D data
// ----------

// no scalar added to pointset
template<class T>
void bpgl_heightmap<T>::pointset_from_tri(
    const vil_image_view<T>& tri_3d,
    vgl_pointset_3d<T>& ptset_output)
{
  vil_image_view<T> empty_scalar;
  this->_pointset_from_tri(
      tri_3d, empty_scalar, ptset_output, true);
}

// add scalar to pointset
template<class T>
void bpgl_heightmap<T>::pointset_from_tri(
    const vil_image_view<T>& tri_3d,
    const vil_image_view<T>& scalar,
    vgl_pointset_3d<T>& ptset_output)
{
  this->_pointset_from_tri(
      tri_3d, scalar, ptset_output, false);
}

// private function, scalar usage controlled by "ignore_scalar"
template<class T>
void bpgl_heightmap<T>::_pointset_from_tri(
    const vil_image_view<T>& tri_3d,
    const vil_image_view<T>& scalar,
    vgl_pointset_3d<T>& ptset_output,
    bool ignore_scalar)
{
  // bounds with tolerance (avoid any floating point error in comparison)
  T tol = 1e-3;
  auto bounds_with_tolerance = heightmap_bounds_;
  bounds_with_tolerance.expand_about_centroid(tol);

  // add triangulated points to pointset
  for (size_t j=0; j < tri_3d.nj(); ++j) {
    for (size_t i=0; i < tri_3d.ni(); ++i) {
      if (vnl_math::isfinite(tri_3d(i,j,0)) &&
          vnl_math::isfinite(tri_3d(i,j,1)) &&
          vnl_math::isfinite(tri_3d(i,j,2)) )
      {
        // confirm 3D point is within bounds
        vgl_point_3d<T> point(tri_3d(i,j,0), tri_3d(i,j,1), tri_3d(i,j,2));
        if(!bounds_with_tolerance.contains(point))
          continue;

        // add point/scalar to pointset
        if (ignore_scalar) {
          ptset_output.add_point(point);
        } else {
          T value = scalar(i,j);
          ptset_output.add_point_with_scalar(point, value);
        }
      }
    }
  }

}


// ----------
// Heightmap from 3D pointset
// Transform 3D pointset to 2D heightmap
// ----------

// without scalar output
template<class T>
void bpgl_heightmap<T>::heightmap_from_pointset(
    const vgl_pointset_3d<T>& ptset,
    vil_image_view<T>& heightmap_output)
{
  vil_image_view<T> empty_scalar_output;
  this->_heightmap_from_pointset(
      ptset, heightmap_output, empty_scalar_output, true);
}

// with scalar output
template<class T>
void bpgl_heightmap<T>::heightmap_from_pointset(
    const vgl_pointset_3d<T>& ptset,
    vil_image_view<T>& heightmap_output,
    vil_image_view<T>& scalar_output)
{
  this->_heightmap_from_pointset(
      ptset, heightmap_output, scalar_output, false);
}

// private function, scalar usage controlled by "ignore_scalar"
template<class T>
void bpgl_heightmap<T>::_heightmap_from_pointset(
    const vgl_pointset_3d<T>& ptset,
    vil_image_view<T>& heightmap_output,
    vil_image_view<T>& scalar_output,
    bool ignore_scalar)
{
  // check pointset sufficency
  if (ptset.npts() < min_neighbors_) {
    throw std::runtime_error("Not enough points in pointset for interpolation");
  }

  // pointset as vectors
  std::vector<vgl_point_2d<T> > triangulated_xy;
  std::vector<T> height_vals;

  for (const auto& point_3d : ptset.points()) {
    vgl_point_2d<T> point_2d(point_3d.x(), point_3d.y());
    triangulated_xy.emplace_back(point_2d);
    height_vals.emplace_back(point_3d.z());
  }

  // image upper left & size
  // image must contain all samples within bounds, inclusive
  vgl_point_2d<T> upper_left(heightmap_bounds_.min_x(), heightmap_bounds_.max_y());
  size_t ni = static_cast<size_t>(std::floor(heightmap_bounds_.width() / ground_sample_distance_ + 1));
  size_t nj = static_cast<size_t>(std::floor(heightmap_bounds_.height() / ground_sample_distance_ + 1));

  // maximum neighbor distance
  T max_dist = neighbor_dist_factor_ * ground_sample_distance_;

  // default interpolation function
  bpgl_gridding::linear_interp<T,T> interp_fun;

  // heightmap gridding
  heightmap_output = bpgl_gridding::grid_data_2d(
      interp_fun,
      triangulated_xy, height_vals,
      upper_left, ni, nj, ground_sample_distance_,
      min_neighbors_, max_neighbors_, max_dist);

  // bounds check to remove outliers
  T min_z = heightmap_bounds_.min_z();
  T max_z = heightmap_bounds_.max_z();

  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      if ((heightmap_output(i,j) < min_z) || (heightmap_output(i,j) > max_z)) {
        heightmap_output(i,j) = NAN;
      }
    }
  }

  // scalar interpolation
  if (!ignore_scalar) {

    // scalar gridding
    scalar_output = bpgl_gridding::grid_data_2d(
        interp_fun,
        triangulated_xy, ptset.scalars(),
        upper_left, ni, nj, ground_sample_distance_,
        min_neighbors_, max_neighbors_, max_dist);

    // remove scalar without corresponding height
    for (int j=0; j<nj; ++j) {
      for (int i=0; i<ni; ++i) {
        if (!vnl_math::isfinite(heightmap_output(i,j))) {
          scalar_output(i,j) = NAN;
        }
      }
    }

  } // end scalar interpolation

}


// ----------
// Heightmap from triangulated input
// Convert triangulated input directly to heigtmap
// ----------

// without scalar output
template<class T>
void bpgl_heightmap<T>::heightmap_from_tri(
    const vil_image_view<T>& tri_3d,
    vil_image_view<T>& heightmap_output)
{
  vgl_pointset_3d<T> ptset;
  this->pointset_from_tri(
      tri_3d, ptset);
  this->heightmap_from_pointset(
      ptset, heightmap_output);
}

// with scalar output
template<class T>
void bpgl_heightmap<T>::heightmap_from_tri(
    const vil_image_view<T>& tri_3d,
    const vil_image_view<T>& scalar,
    vil_image_view<T>& heightmap_output,
    vil_image_view<T>& scalar_output)
{
  vgl_pointset_3d<T> ptset;
  this->pointset_from_tri(
      tri_3d, scalar, ptset);
  this->heightmap_from_pointset(
      ptset, heightmap_output, scalar_output);
}


// ----------
// CLEANUP
// ----------

// macros for Templates
#undef BPGL_HEIGHTMAP_INSTANIATE
#define BPGL_HEIGHTMAP_INSTANIATE(T) \
template class bpgl_heightmap<T>

#undef BPGL_HEIGHTMAP_FROM_DISPARITY_INSTANIATE
#define BPGL_HEIGHTMAP_FROM_DISPARITY_INSTANIATE(T, CAM_T) \
template vil_image_view<T> bpgl_heightmap_from_disparity<T, CAM_T>( \
    CAM_T const& cam1, \
    CAM_T const& cam2, \
    vil_image_view<T> const& disparity, \
    vgl_box_3d<T> heightmap_bounds, \
    T ground_sample_distance)

#endif
