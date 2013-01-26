#include "volm_spherical_region_query.h"
#include "volm_camera_space.h"
#include <bpgl/depth_map/depth_map_scene.h>
#include "volm_spherical_container.h"
#include <bsol/bsol_algs.h>
#include <vgl/vgl_polygon.h>
#include <vcl_vector.h>
volm_spherical_region_query::
volm_spherical_region_query(depth_map_scene_sptr const& dm_scene,
			    volm_camera_space_sptr const& cam_space,
			    volm_spherical_container_sptr const& sph_vol):
  dm_scene_(dm_scene), cam_space_(cam_space), sph_vol_(sph_vol)
{    
  this->construct_spherical_regions();
}

void volm_spherical_region_query::construct_spherical_regions(){
  // roll affects the shape of the axis-aligned boxes on the sphere
  double roll_start = cam_space_->roll_mid()-cam_space_->roll_radius();
  double rinc = cam_space_->roll_inc();
  unsigned n_roll = cam_space_->n_roll();
  unsigned roll_idx = 0;
  for( roll_idx = 0; roll_idx < n_roll; ++roll_idx){
    // camera at indicated roll and other rotations zero.
    unsigned cam_indx = cam_space_->cam_index(roll_idx, 0, 0, 0);
    vpgl_perspective_camera<double> cam = cam_space_->camera(cam_indx);
    
    // ====  construct object regions =====
    vcl_vector<depth_map_region_sptr> object_regions =
      dm_scene_->scene_regions();
    unsigned n_regions = object_regions.size();
    for(unsigned reg_idx = 0; reg_idx<n_regions; ++reg_idx){
      depth_map_region_sptr obj_reg = object_regions[reg_idx];
      volm_spherical_query_region sph_reg;
      sph_reg.set_from_depth_map_region(cam, obj_reg, sph_vol_);
      sph_regions_[roll_idx].push_back(sph_reg);
    }
    vcl_vector<depth_map_region_sptr> sky_regions =
      dm_scene_->sky();
    for(vcl_vector<depth_map_region_sptr>::iterator sit = sky_regions.begin();
	sit != sky_regions.end(); ++sit){
      depth_map_region_sptr sky_reg = *sit;
      volm_spherical_query_region sph_reg;
      sph_reg.set_from_depth_map_region(cam, sky_reg, sph_vol_);
      sph_regions_[roll_idx].push_back(sph_reg);
    }
    vcl_vector<depth_map_region_sptr> gp_regions =
      dm_scene_->ground_plane();
    for(vcl_vector<depth_map_region_sptr>::iterator git = gp_regions.begin();
	git != gp_regions.end(); ++git){
      depth_map_region_sptr gp_reg = *git;
      volm_spherical_query_region sph_reg;
      sph_reg.set_from_depth_map_region(cam, gp_reg, sph_vol_);
      sph_regions_[roll_idx].push_back(sph_reg);
    }
  }
}
