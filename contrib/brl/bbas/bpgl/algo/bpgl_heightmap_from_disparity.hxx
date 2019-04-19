// This is contrib/brl/bbas/bpgl/algo/bpgl_heightmap_from_disparity.hxx
#ifndef bpgl_heightmap_from_disparity_hxx_
#define bpgl_heightmap_from_disparity_hxx_

#include "bpgl_3d_from_disparity.h"

#include "bpgl_heightmap_from_disparity.h"
#include "bpgl_gridding.h"

#include <vnl/vnl_math.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>

template<class CAM_T>
vil_image_view<float>
bpgl_heightmap_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                              vil_image_view<float> disparity, vgl_box_3d<double> heightmap_bounds,
                              double ground_sample_distance)
{
  // convert disparity to set of 3D points
  vil_image_view<float> triangulated = bpgl_3d_from_disparity(cam1, cam2, disparity);

  // put triangulated points into a vector
  std::vector<vgl_point_2d<double>> triangulated_xy;
  std::vector<float> height_vals;

  const float min_z = heightmap_bounds.min_z();
  const float max_z = heightmap_bounds.max_z();

  for (size_t j=0; j<triangulated.nj(); ++j) {
    for (size_t i=0; i<triangulated.ni(); ++i) {
      if (vnl_math::isfinite(triangulated(i,j,0)) &&
          vnl_math::isfinite(triangulated(i,j,1)) &&
          vnl_math::isfinite(triangulated(i,j,2)) ) {
        const float z = triangulated(i,j,2);
        if ((z < min_z) || (z > max_z)) {
          continue;
        }
        // x,y goes in ground samples
        triangulated_xy.emplace_back(triangulated(i,j,0),
                                     triangulated(i,j,1));
        // z coordinates are what we want to grid
        height_vals.push_back(z);
      }
    }
  }

  vgl_point_2d<double> upper_left(heightmap_bounds.min_x(), heightmap_bounds.max_y());

  // set ni,nj such that image contains all samples within bounds, inclusive
  size_t ni = static_cast<unsigned>(std::floor(heightmap_bounds.width() / ground_sample_distance + 1));
  size_t nj = static_cast<unsigned>(std::floor(heightmap_bounds.height() / ground_sample_distance + 1));
  unsigned num_neighbors = 3;

  // grid the 3D points into a DSM
  bpgl_gridding::linear_interp<double, float> interp_fun(ground_sample_distance*3, NAN);
  vil_image_view<float> hmap = bpgl_gridding::grid_data_2d(triangulated_xy, height_vals,
                                                           upper_left, ni, nj, ground_sample_distance,
                                                           interp_fun, num_neighbors);

  // final bounds check to remove outliers
  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      if ((hmap(i,j) < min_z) || (hmap(i,j) > max_z)) {
        hmap(i,j) = NAN;
      }
    }
  }

  return hmap;
}

template<class pointT, class CAM_T>
void bpgl_pointset_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                                  vil_image_view<float> disparity,
                                  vgl_box_3d<pointT> heightmap_bounds,
                                  std::vector<vgl_point_3d<pointT> >& ptset){
  ptset.clear();
  vgl_box_2d<pointT> bounds_2d;
  vgl_point_2d<pointT> pmin(heightmap_bounds.min_x(), heightmap_bounds.min_y());
  vgl_point_2d<pointT> pmax(heightmap_bounds.max_x(), heightmap_bounds.max_y());
  bounds_2d.add(pmin);   bounds_2d.add(pmax);
  // convert disparity to set of 3D points
  vil_image_view<float> triangulated = bpgl_3d_from_disparity(cam1, cam2, disparity);
  // put triangulated points into a vector
  std::vector<vgl_point_2d<float> > triangulated_xy;
  std::vector<float> height_vals;

  const float min_z = heightmap_bounds.min_z();
  const float max_z = heightmap_bounds.max_z();

  for (size_t j=0; j<triangulated.nj(); ++j) {
    for (size_t i=0; i<triangulated.ni(); ++i) {
      if (vnl_math::isfinite(triangulated(i,j,0)) &&
          vnl_math::isfinite(triangulated(i,j,1)) &&
          vnl_math::isfinite(triangulated(i,j,2)) ) {
        const float z = triangulated(i,j,2);
        if ((z < min_z) || (z > max_z)) {
          continue;
        }
        pointT x = static_cast<pointT>(triangulated(i, j, 0));
        pointT y = static_cast<pointT>(triangulated(i, j, 1));
        vgl_point_2d<pointT> pxy(x, y);
        if(!bounds_2d.contains(pxy))
          continue;
        ptset.emplace_back(x, y, z);
      }
    }
  }
}

#define BPGL_HEIGHTMAP_FROM_DISPARITY_INSTANIATE(CAM_T) \
template vil_image_view<float> \
bpgl_heightmap_from_disparity<CAM_T>(CAM_T const& cam1, CAM_T const& cam2, \
                                     vil_image_view<float> disparity, \
                                     vgl_box_3d<double> heightmap_bounds, \
                                     double ground_sample_distance)

#define BPGL_POINTSET_FROM_DISPARITY_INSTANIATE(CAM_T, pointT) \
template void bpgl_pointset_from_disparity(CAM_T const& cam1, CAM_T const& cam2, \
                                           vil_image_view<float> disparity, \
                                           vgl_box_3d<pointT> heightmap_bounds, \
                                           std::vector<vgl_point_3d<pointT> >& )

#endif
