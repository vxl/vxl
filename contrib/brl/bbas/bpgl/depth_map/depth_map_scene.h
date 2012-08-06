//This is brl/bbas/bpgl/depth_map/depth_map_scene.h
#ifndef depth_map_scene_h_
#define depth_map_scene_h_
//:
// \file
// \brief A class to represent a symbolic depth map
//
// \author J. L. Mundy
// \date July 31, 2012
// \verbatim
//  Modifications
// \endverbatim
// The idea is that the absolute depth map for a scene cannot often 
// be extracted from a single image since there can be unknown parameters.
// For example, an object depth in the image cannot be known unless the
// camera is known as well as the 3-d dimensions of the object. Moreover,
// the object can have surface depth variations that may not be able to
// be recovered, e.g. shape from shading is fragile.
//
// Instead it is necessary to express the depth map analytically in terms 
// of regions whose depth is a parameter. For a given specification of depth,
// the lateral extent of the region in 3-d is determined. In this 
// initial version the scene will be represented by a ground plane and
// regions perpendicular to the ground plane and the viewing direction,
// bounded by occluding contours. By convention, the camera principal
// ray is along the world Y axis but can be inclined with respect to the
// ground plane. The image can be rotated about the principal ray as well.
// These orientations are defined by the camera model for the image.
#include <vcl_vector.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include "depth_map_region_sptr.h"
#include "depth_map_region.h"
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_limits.h>
#include <vcl_map.h>
class depth_map_scene
{
 public:
  depth_map_scene() : ni_(0), nj_(0), ground_plane_(0)
  {}

  depth_map_scene(unsigned ni, unsigned nj) :
    ni_(ni), nj_(nj), ground_plane_(0) {}

  depth_map_scene(unsigned ni, unsigned nj, 
                  vpgl_perspective_camera<double> const& cam,
                  depth_map_region_sptr const& ground_plane,
                  vcl_vector<depth_map_region_sptr> const& scene_regions);

  void set_ground_plane(vsol_polygon_2d_sptr ground_plane,
                        double min_distance, double max_distance);

  void add_region(vsol_polygon_2d_sptr const& region,
                  vgl_vector_3d<double> plane_normal,
                  double min_distance,
                  double max_distance,
                  vcl_string name,
                  depth_map_region::orientation orient);

  vil_image_view<double> depth_map();
  
 protected:
  unsigned ni_, nj_;//: depth map dimensions
  vcl_map<vcl_string, depth_map_region_sptr> scene_regions_;
  depth_map_region_sptr ground_plane_;
  vpgl_perspective_camera<double> cam_;
};
#endif //depth_map_scene_h_
