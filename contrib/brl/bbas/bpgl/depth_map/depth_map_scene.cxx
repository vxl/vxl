#include "depth_map_scene.h"
#include "depth_map_region.h"
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_limits.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vsol/vsol_polygon_3d.h>

depth_map_scene::
depth_map_scene(unsigned ni, unsigned nj,
                vpgl_perspective_camera<double> const& cam,
                depth_map_region_sptr const& ground_plane,
                vcl_vector<depth_map_region_sptr> const& scene_regions)
: ni_(ni), nj_(nj), cam_(cam), ground_plane_(ground_plane)
{
  for (vcl_vector<depth_map_region_sptr>::const_iterator rit = scene_regions.begin();
       rit != scene_regions.end(); ++rit)
    scene_regions_[(*rit)->name()]=(*rit);
}

void depth_map_scene::set_ground_plane(vsol_polygon_2d_sptr ground_plane,
                                       double min_depth,
                                       double max_depth)
{
  vgl_plane_3d<double> gp(0.0, 0.0, 1.0, 0.0);//z axis is the plane normal
  ground_plane_ = new depth_map_region(ground_plane, gp, min_depth,
                                       max_depth, vcl_string("ground_plane"),
                                       depth_map_region::GROUND_PLANE);
}

void depth_map_scene::add_region(vsol_polygon_2d_sptr const& region,
                                 vgl_vector_3d<double> region_normal,
                                 double min_depth,
                                 double max_depth,
                                 vcl_string name,
                                 depth_map_region::orientation orient)
{
  vgl_plane_3d<double> plane(region_normal.x(), region_normal.y(), region_normal.z(), 0.0);
  depth_map_region_sptr reg = new depth_map_region(region, plane,
                                                   min_depth, max_depth,
                                                   name,
                                                   orient);
  scene_regions_[name]=reg;
}

vil_image_view<double> depth_map_scene::depth_map()
{
  assert(ground_plane_);
  vil_image_view<double> depth(ni_, nj_);
  depth.fill(-1.0); // depth is undefined
  // add the ground plane to the depth image
  bool good = ground_plane_->update_depth_image(depth, cam_);
  assert(good);
  vcl_map<vcl_string, depth_map_region_sptr>::iterator rit = scene_regions_.begin();
  for (; rit!= scene_regions_.end(); ++rit) {
    good = (*rit).second->update_depth_image(depth, cam_);
    assert(good);
  }
  return depth;
}
