// This is contrib/brl/bbas/bpgl/algo/bpgl_heightmap_from_disparity.hxx
#ifndef bpgl_heightmap_from_disparity_hxx_
#define bpgl_heightmap_from_disparity_hxx_

#include "bpgl_3d_from_disparity.h"

#include "bpgl_heightmap_from_disparity.h"
#include "bpgl_gridding.h"

#include <vnl/vnl_math.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>

//: compute the heightmap given that triangulated 3-d data is available
template<class CAM_T>
vil_image_view<float>
bpgl_heightmap_from_tri_image(CAM_T const& cam1, CAM_T const& cam2, vil_image_view<float> const& tri_image_3d,
                              vgl_box_3d<double> heightmap_bounds,
                              double ground_sample_distance)
{
  // put triangulated points into a vector
  std::vector<vgl_point_2d<double>> triangulated_xy;
  std::vector<float> height_vals;

  const float min_z = heightmap_bounds.min_z();
  const float max_z = heightmap_bounds.max_z();

  for (size_t j=0; j<tri_image_3d.nj(); ++j) {
    for (size_t i=0; i<tri_image_3d.ni(); ++i) {
      if (vnl_math::isfinite(tri_image_3d(i,j,0)) &&
          vnl_math::isfinite(tri_image_3d(i,j,1)) &&
          vnl_math::isfinite(tri_image_3d(i,j,2)) ) {
        const float z = tri_image_3d(i,j,2);
        if ((z < min_z) || (z > max_z)) {
          continue;
        }
        // x,y goes in ground samples
        triangulated_xy.emplace_back(tri_image_3d(i,j,0),
                                     tri_image_3d(i,j,1));
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

//: compute the heightmap using dispairity to compute the triangulated 3-d data
template<class CAM_T>
vil_image_view<float>
bpgl_heightmap_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                              vil_image_view<float> const& disparity, vgl_box_3d<double> heightmap_bounds,
                              double ground_sample_distance)
{
  // convert disparity to set of 3D points
  vil_image_view<float> triangulated = bpgl_3d_from_disparity(cam1, cam2, disparity);
  return bpgl_heightmap_from_tri_image(cam1, cam2, triangulated, heightmap_bounds, ground_sample_distance);
}

//: compute the heightmap and scalar map given that triangulated 3-d data is available
template<class CAM_T>
void bpgl_heightmap_with_scalar_from_tri_image(CAM_T const& cam1, CAM_T const& cam2,
                                               vil_image_view<float> const& tri_image_3d, vil_image_view<float> scalar,
                                               vgl_box_3d<double> heightmap_bounds, double ground_sample_distance,
                                               vil_image_view<float>& heightmap, vil_image_view<float>& scalar_map)
{
  // put triangulated points into a vector
  std::vector<vgl_point_2d<double>> triangulated_xy;
  std::vector<float> height_vals, scalar_vals;

  const float min_z = heightmap_bounds.min_z();
  const float max_z = heightmap_bounds.max_z();
  for (size_t j=0; j<tri_image_3d.nj(); ++j) {
    for (size_t i=0; i<tri_image_3d.ni(); ++i) {
      if (vnl_math::isfinite(tri_image_3d(i,j,0)) &&
          vnl_math::isfinite(tri_image_3d(i,j,1)) &&
          vnl_math::isfinite(tri_image_3d(i,j,2)) &&
          vnl_math::isfinite(tri_image_3d(i,j,3))
          ) {
        const float z = tri_image_3d(i,j,2);
        const float s = tri_image_3d(i,j,3);
        if ((z < min_z) || (z > max_z)) {
          continue;
        }
        // x,y goes in ground samples
        triangulated_xy.emplace_back(tri_image_3d(i,j,0),
                                     tri_image_3d(i,j,1));
        // z coordinates are what we want to grid
        height_vals.push_back(z);
        scalar_vals.push_back(s);
      }
    }
  }
  vgl_point_2d<double> upper_left(heightmap_bounds.min_x(), heightmap_bounds.max_y());

  // set ni,nj such that image contains all samples within bounds, inclusive
  size_t ni = static_cast<unsigned>(std::floor(heightmap_bounds.width() / ground_sample_distance + 1));
  size_t nj = static_cast<unsigned>(std::floor(heightmap_bounds.height() / ground_sample_distance + 1));
  unsigned num_neighbors = 5; //need more smoothing for prob values (compare to 3 for z vals)

  // grid the 3D points into a DSM
  bpgl_gridding::linear_interp<double, float> interp_fun(ground_sample_distance*3, NAN);

  heightmap = bpgl_gridding::grid_data_2d(triangulated_xy, height_vals,
                                          upper_left, ni, nj, ground_sample_distance,
                                          interp_fun, num_neighbors);

  // final bounds check to remove outliers
  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      if ((heightmap(i,j) < min_z) || (heightmap(i,j) > max_z)) {
        heightmap(i,j) = NAN;
      }
    }
  }
  scalar_map = bpgl_gridding::grid_data_2d(triangulated_xy, scalar_vals,
                                           upper_left, ni, nj, ground_sample_distance,
                                           interp_fun, num_neighbors);
}

//: compute the heightmap and scalar map using disparity to compute triangulated 3-d data
template<class CAM_T>
void bpgl_heightmap_with_scalar_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                                               vil_image_view<float> const& disparity, vil_image_view<float> scalar,
                                               vgl_box_3d<double> heightmap_bounds, double ground_sample_distance,
                                               vil_image_view<float>& heightmap, vil_image_view<float>& scalar_map)
{
  // convert disparity to set of 3D points
  vil_image_view<float> triangulated = bpgl_3d_from_disparity_with_scalar(cam1, cam2, disparity, scalar);
  bpgl_heightmap_with_scalar_from_tri_image(cam1, cam2, triangulated, scalar, heightmap_bounds, ground_sample_distance,
                                            heightmap, scalar_map);
}

//:compute the 3-d pointset given that triangulated 3-d data is available
template<class pointT, class CAM_T>
void bpgl_pointset_from_tri_image(CAM_T const& cam1, CAM_T const& cam2,
                                  vil_image_view<float> const& tri_image_3d,
                                  vgl_box_3d<pointT> heightmap_bounds,
                                  std::vector<vgl_point_3d<pointT> >& ptset)
{
  ptset.clear();
  vgl_box_2d<pointT> bounds_2d;
  vgl_point_2d<pointT> pmin(heightmap_bounds.min_x(), heightmap_bounds.min_y());
  vgl_point_2d<pointT> pmax(heightmap_bounds.max_x(), heightmap_bounds.max_y());
  bounds_2d.add(pmin);   bounds_2d.add(pmax);

  // put triangulated points into a vector
  std::vector<vgl_point_2d<float> > triangulated_xy;
  std::vector<float> height_vals;

  const float min_z = heightmap_bounds.min_z();
  const float max_z = heightmap_bounds.max_z();

  for (size_t j=0; j<tri_image_3d.nj(); ++j) {
    for (size_t i=0; i<tri_image_3d.ni(); ++i) {
      if (vnl_math::isfinite(tri_image_3d(i,j,0)) &&
          vnl_math::isfinite(tri_image_3d(i,j,1)) &&
          vnl_math::isfinite(tri_image_3d(i,j,2)) ) {
        const float z = tri_image_3d(i,j,2);
        if ((z < min_z) || (z > max_z)) {
          continue;
        }
        pointT x = static_cast<pointT>(tri_image_3d(i, j, 0));
        pointT y = static_cast<pointT>(tri_image_3d(i, j, 1));
        vgl_point_2d<pointT> pxy(x, y);
        if(!bounds_2d.contains(pxy))
          continue;
        ptset.emplace_back(x, y, z);
      }
    }
  }
}

//: compute the 3-d pointset using disparity to compute triangulated 3-d data
template<class pointT, class CAM_T>
void bpgl_pointset_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                                  vil_image_view<float> const& disparity,
                                  vgl_box_3d<pointT> heightmap_bounds,
                                  std::vector<vgl_point_3d<pointT> >& ptset)
{
  ptset.clear();
  vgl_box_2d<pointT> bounds_2d;
  vgl_point_2d<pointT> pmin(heightmap_bounds.min_x(), heightmap_bounds.min_y());
  vgl_point_2d<pointT> pmax(heightmap_bounds.max_x(), heightmap_bounds.max_y());
  bounds_2d.add(pmin);   bounds_2d.add(pmax);
  // convert disparity to set of 3D points
  vil_image_view<float> triangulated = bpgl_3d_from_disparity(cam1, cam2, disparity);
  bpgl_pointset_from_tri_image(cam1, cam2, triangulated, heightmap_bounds, ptset);
}

#define BPGL_HEIGHTMAP_FROM_DISPARITY_INSTANIATE(CAM_T) \
template vil_image_view<float>                                          \
bpgl_heightmap_from_tri_image<CAM_T>(CAM_T const& cam1, CAM_T const& cam2, \
                                     vil_image_view<float> const& tri_image, \
                                     vgl_box_3d<double> heightmap_bounds, \
                                     double ground_sample_distance);    \
template vil_image_view<float>                                          \
bpgl_heightmap_from_disparity<CAM_T>(CAM_T const& cam1, CAM_T const& cam2, \
                                     vil_image_view<float> const& disparity, \
                                     vgl_box_3d<double> heightmap_bounds, \
                                     double ground_sample_distance);    \
template void bpgl_heightmap_with_scalar_from_tri_image<CAM_T>(CAM_T const& cam1, CAM_T const& cam2, \
                                               vil_image_view<float> const& tri_image, vil_image_view<float> scalar,\
                                               vgl_box_3d<double> heightmap_bounds, double ground_sample_distance,\
                                               vil_image_view<float>& heightmap, vil_image_view<float>& scalar_map); \
template void bpgl_heightmap_with_scalar_from_disparity<CAM_T>(CAM_T const& cam1, CAM_T const& cam2, \
                                               vil_image_view<float> const& disparity, vil_image_view<float> scalar,\
                                               vgl_box_3d<double> heightmap_bounds, double ground_sample_distance,\
                                               vil_image_view<float>& heightmap, vil_image_view<float>& scalar_map)


#define BPGL_POINTSET_FROM_DISPARITY_INSTANIATE(CAM_T, pointT)          \
template void bpgl_pointset_from_tri_image(CAM_T const& cam1, CAM_T const& cam2, \
                                           vil_image_view<float> const& tri_image_3d, \
                                           vgl_box_3d<pointT> heightmap_bounds, \
                                           std::vector<vgl_point_3d<pointT> >& ); \
template void bpgl_pointset_from_disparity(CAM_T const& cam1, CAM_T const& cam2, \
                                           vil_image_view<float> const& disparity, \
                                           vgl_box_3d<pointT> heightmap_bounds, \
                                           std::vector<vgl_point_3d<pointT> >& )
#endif
