#include <volm/conf/volm_conf_query.h>
//:
// \file
#include <vcl_cassert.h>
#include <bsol/bsol_algs.h>

// default constructor
volm_conf_query::volm_conf_query()
{
  ni_ = 0;  nj_ = 0;  ncam_ = 0;  nobj_ = 0;  nref_ = 0;
  dm_ = 0;
  cameras_.clear();  camera_strings_.clear();
  ref_obj_name_.clear();  obj_3d_polygons_.clear();  obj_land_ids_.clear();  conf_objects_.clear();
}

// create query from labeled depth map scene
volm_conf_query::volm_conf_query(volm_camera_space_sptr cam_space, depth_map_scene_sptr depth_scene)
{
  dm_ = depth_scene;
  ni_ = dm_->ni();  nj_ = dm_->nj();

  // create reference objects from depth scene
  bool success = this->parse_ref_object(dm_);
  assert(success && "volm_conf_query: parse reference object list from depth scene failed");
  nref_ = this->ref_obj_name_.size();
  // create cameras
  success = this->create_perspective_cameras(cam_space);
  assert(success && "volm_conf_query: construct perspective cameras from camera space failed");
  ncam_ = this->cameras_.size();
  // construct 3d polygons from depth scene for each camera
  success = this->create_3d_polygons(dm_);
  assert(success && "volm_conf_query: construct 3-d polygon failed");
  nobj_ = this->obj_3d_polygons_.size();
  // construct configurational object from 3d polygons
  success  = this->create_conf_object();
  assert(success && "volm_conf_query: construct configurational objects failed");
}

bool volm_conf_query::parse_ref_object(depth_map_scene_sptr dm)
{
  ref_obj_name_.clear();
  if (dm == 0)
    return false;
  // note that sky object should never be reference object
  if (dm->ground_plane().size())
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      if ( dm->ground_plane()[i]->is_ref() )
        ref_obj_name_.push_back(dm->ground_plane()[i]->name());
  if (dm->scene_regions().size())
    for (unsigned i = 0; i < dm->scene_regions().size(); i++)
      if ( dm->scene_regions()[i]->is_ref() )
        ref_obj_name_.push_back(dm->scene_regions()[i]->name());
  if (ref_obj_name_.empty())
    return false;
  return true;
}

bool volm_conf_query::create_perspective_cameras(volm_camera_space_sptr cam_space)
{
  // iterate over valid cameras in the camera space
  // Note that ground plane construct has been applied on camera space
  cameras_.clear();  camera_strings_.clear();  camera_angles_.clear();
  vcl_vector<unsigned> const& valid_cams = cam_space->valid_indices();
  for (unsigned i = 0; i < valid_cams.size(); i++) {
    vpgl_perspective_camera<double> cam = cam_space->camera(valid_cams[i]);
    vcl_string cam_str = cam_space->get_string(valid_cams[i]);
    cameras_.push_back(cam);
    camera_strings_.push_back(cam_str);
    camera_angles_.push_back(cam_space->camera_angles(valid_cams[i]));
  }
  return true;
}

// construct 3-d polygons from depth map scene for each camera
bool volm_conf_query::create_3d_polygons(depth_map_scene_sptr dm)
{
  obj_3d_polygons_.clear();
  //vcl_vector<vcl_map<vcl_string, vcl_vector<vgl_point_3d<float> > > > obj_3d_polygons_;

  for (unsigned cam_id = 0;  cam_id < ncam_;  cam_id++) {
    vpgl_perspective_camera<double> cam = cameras_[cam_id];
    // get the camera center
    vgl_homg_point_3d<double> homg_cam_center = cam.camera_center();
    double cx, cy, cz;
    if(!homg_cam_center.get_nonhomogeneous(cx, cy,cz));
      return false;
    vgl_point_3d<double> cam_center(cx, cy, cz);
    vcl_vector<depth_map_region_sptr> grd = dm_->ground_plane();
    for (unsigned i = 0; i < grd.size(); i++) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly(grd[i]->region_2d());
    }
  }


  return true;
}

bool volm_conf_query::create_conf_object()
{
  return true;
}