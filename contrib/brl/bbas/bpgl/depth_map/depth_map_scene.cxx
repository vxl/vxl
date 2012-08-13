#include "depth_map_scene.h"
//:
// \file
#include "depth_map_region.h"
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_limits.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsl/vsl_map_io.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>

depth_map_scene::
depth_map_scene(unsigned ni, unsigned nj,
                vpgl_perspective_camera<double> const& cam,
                depth_map_region_sptr const& ground_plane,
                depth_map_region_sptr const& sky,
                vcl_vector<depth_map_region_sptr> const& scene_regions)
  : ni_(ni), nj_(nj), ground_plane_(ground_plane), sky_(sky), cam_(cam)
{
  for (vcl_vector<depth_map_region_sptr>::const_iterator rit = scene_regions.begin();
       rit != scene_regions.end(); ++rit)
    scene_regions_[(*rit)->name()]=(*rit);
}

void depth_map_scene::set_ground_plane(vsol_polygon_2d_sptr ground_plane)
{
  vgl_plane_3d<double> gp(0.0, 0.0, 1.0, 0.0);//z axis is the plane normal

  ground_plane_ = new depth_map_region(ground_plane, gp,
                                       vcl_string("ground_plane"),
                                       depth_map_region::GROUND_PLANE);
}

void depth_map_scene::set_sky(vsol_polygon_2d_sptr sky)
{
  sky_ = new depth_map_region(sky, "sky");
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


void depth_map_scene::
  add_ortho_perp_region(vsol_polygon_2d_sptr const& region,
                        double min_depth, double max_depth,
                        vcl_string name)
{
  vgl_vector_3d<double> normal_dir = depth_map_region::perp_ortho_dir(cam_);
  this->add_region(region, normal_dir, min_depth, max_depth, name,
                   depth_map_region::VERTICAL);
}

vcl_vector<depth_map_region_sptr> depth_map_scene::scene_regions() const
{
  vcl_vector<depth_map_region_sptr> ret;
  vcl_map<vcl_string, depth_map_region_sptr>::const_iterator rit = scene_regions_.begin();
  for (; rit != scene_regions_.end(); ++rit)
    ret.push_back((*rit).second);
  return ret;
}

bool depth_map_scene::set_depth(double depth, vcl_string const& name)
{
  if (name == "sky" || name == "ground_plane")
    return false;
  depth_map_region_sptr r = scene_regions_[name];
  if (!r) return false;
  if (r->orient_type() != depth_map_region::VERTICAL)
    return false;
  r->set_region_3d(depth, cam_);
  return true;
}

void depth_map_scene::
set_ground_plane_max_depth(double max_depth,
                           double proximity_scale_factor)
{
  if (ground_plane_)
    ground_plane_->set_ground_plane_max_depth(max_depth, cam_,
                                              proximity_scale_factor);
}

vil_image_view<float> depth_map_scene::depth_map()
{
  assert(ground_plane_);
  vil_image_view<float> depth(ni_, nj_);
  depth.fill(-1.0f); // depth is undefined
  // add the ground plane to the depth image

  bool good = ground_plane_->update_depth_image(depth, cam_);
  good = good && sky_->update_depth_image(depth, cam_);

  assert(good);
  vcl_map<vcl_string, depth_map_region_sptr>::iterator rit = scene_regions_.begin();
  for (; rit!= scene_regions_.end(); ++rit) {
    good = (*rit).second->update_depth_image(depth, cam_);
    assert(good);
  }
  return depth;
}

//: binary IO write
void depth_map_scene::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, ni_);
  vsl_b_write(os, nj_);
  vsl_b_write(os, scene_regions_);
  vsl_b_write(os, ground_plane_);
  vsl_b_write(os, sky_);
  vsl_b_write(os, cam_);
}

//: binary IO read
void depth_map_scene::b_read(vsl_b_istream& is)
{
  vsl_b_read(is, ni_);
  vsl_b_read(is, nj_);
  vsl_b_read(is, scene_regions_);
  vsl_b_read(is, ground_plane_);
  vsl_b_read(is, sky_);
  vsl_b_read(is, cam_);
}

void vsl_b_write(vsl_b_ostream& os, const depth_map_scene* ds_ptr)
{
  if (ds_ptr ==0)
    vsl_b_write(os, false);
  else
    vsl_b_write(os, true);
  depth_map_scene* ds_non_const = const_cast<depth_map_scene*>(ds_ptr);
  ds_non_const->b_write(os);
}

void vsl_b_read(vsl_b_istream &is, depth_map_scene*& ds_ptr)
{
  bool valid_ptr = false;
  vsl_b_read(is, valid_ptr);
  if (valid_ptr) {
    ds_ptr = new depth_map_scene();
    ds_ptr->b_read(is);
    return;
  }
  ds_ptr = 0;
}

void vsl_b_write(vsl_b_ostream& os, const depth_map_scene_sptr& ds_ptr)
{
  depth_map_scene* ds=ds_ptr.ptr();
  vsl_b_write(os, ds);
}

void vsl_b_read(vsl_b_istream &is, depth_map_scene_sptr& ds_ptr)
{
  depth_map_scene* ds=0;
  vsl_b_read(is, ds);
  ds_ptr = ds;
}
