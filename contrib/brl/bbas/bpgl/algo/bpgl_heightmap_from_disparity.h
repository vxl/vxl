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
 * disparity should contain pixel offsets such that (img1(u, v) <--> img2(u - disparity, v)
 **/
template<class CAM_T>
vil_image_view<float> 
bpgl_heightmap_from_disparity(CAM_T const& cam1, CAM_T const& cam2,
                              vil_image_view<float> disparity, vgl_box_3d<double> heightmap_bounds,
                              double ground_sample_distance);

#endif
