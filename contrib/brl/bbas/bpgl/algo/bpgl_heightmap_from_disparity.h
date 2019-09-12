// This is contrib/brl/bbas/bpgl/algo/bpgl_heightmap_from_disparity.h
#ifndef bpgl_heightmap_from_disparity_h_
#define bpgl_heightmap_from_disparity_h_
//:
// \file
// \brief Methods related to z(x,y) heightmap generation
// \author Dan Crispell
// \date Nov 15, 2018
//

#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_view.h>

/**
 * Given cameras for a stereo pair and a disparity map, return a 3-plane (x,y,z)
 * image containing the corresponding triangulated 3D points in the form z(x,y)
 * disparity should contain pixel offsets such that (img1(u, v) <--> img2(u + disparity, v)
 * for example, given the pixel location (100, 200) in image1, and a disparity value of -20 at that location
 * in the disparity image,x then the corresponding pixel location in image2 is (80, 200).
 **/
template<class CAM_T>
vil_image_view<float>
bpgl_heightmap_from_tri_image(CAM_T const& cam1, CAM_T const& cam2,
                              vil_image_view<float> const& tri_image_3d, vgl_box_3d<double> heightmap_bounds,
                              double ground_sample_distance);

template<class CAM_T>
vil_image_view<float>
bpgl_heightmap_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                              vil_image_view<float> const& disparity, vgl_box_3d<double> heightmap_bounds,
                              double ground_sample_distance);

template<class CAM_T>
void bpgl_heightmap_with_scalar_from_tri_image(CAM_T const& cam1, CAM_T const& cam2,
                                               vil_image_view<float> const& tri_image_3d, vil_image_view<float> scalar,
                                               vgl_box_3d<double> heightmap_bounds, double ground_sample_distance,
                                               vil_image_view<float>& heightmap, vil_image_view<float>& scalar_map);

template<class CAM_T>
void bpgl_heightmap_with_scalar_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                                               vil_image_view<float> const& disparity, vil_image_view<float> scalar,
                                               vgl_box_3d<double> heightmap_bounds, double ground_sample_distance,
                                               vil_image_view<float>& heightmap, vil_image_view<float>& scalar_map);

template<class T, class CAM_T>
void bpgl_pointset_from_tri_image(CAM_T const& cam1, CAM_T const& cam2,
                                  vil_image_view<float> const& tri_image_3d,
                                  vgl_box_3d<T> heightmap_bounds,
                                  std::vector<vgl_point_3d<T> >& ptset);

template<class T, class CAM_T>
void bpgl_pointset_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                                  vil_image_view<float> const& disparity,
                                  vgl_box_3d<T> heightmap_bounds,
                                  std::vector<vgl_point_3d<T> >& ptset);

#endif
