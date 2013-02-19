#include "volm_spherical_region_query.h"
#include "volm_camera_space.h"
#include <vsph/vsph_unit_sphere.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include "volm_spherical_container.h"
#include <bsol/bsol_algs.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

volm_spherical_region_query::
volm_spherical_region_query(depth_map_scene_sptr const& dm_scene,
                            volm_camera_space_sptr const& cam_space,
                            volm_spherical_container_sptr const& sph_vol)
: dm_scene_(dm_scene), cam_space_(cam_space), sph_vol_(sph_vol)
{
  this->construct_spherical_regions();
}

void volm_spherical_region_query::construct_spherical_regions()
{
  unsigned n_roll = cam_space_->n_roll();

  double top_fov = 10.0;
  double head = 90.0, tilt = 90.0;
  unsigned roll_idx = 0;
  for ( roll_idx = 0; roll_idx < n_roll; ++roll_idx) {
    double roll = cam_space_->roll(roll_idx);
    cam_angles cangs(roll, top_fov, head, tilt);
    // camera at indicated roll and other rotations nominal.
    int cam_index = cam_space_->closest_index(cangs);
    assert(cam_index>=0);
    vpgl_perspective_camera<double> cam = cam_space_->camera(cam_index);

    // ====  construct object regions =====
    vcl_vector<depth_map_region_sptr> sky_regions =
      dm_scene_->sky();
    for (vcl_vector<depth_map_region_sptr>::iterator sit = sky_regions.begin();
         sit != sky_regions.end(); ++sit) {
      depth_map_region_sptr sky_reg = *sit;
      volm_spherical_query_region sph_reg;
      sph_reg.set_from_depth_map_region(cam, sky_reg, sph_vol_);
      sph_regions_[roll_idx].push_back(sph_reg);
    }
    vcl_vector<depth_map_region_sptr> gp_regions =
      dm_scene_->ground_plane();
    for (vcl_vector<depth_map_region_sptr>::iterator git = gp_regions.begin();
         git != gp_regions.end(); ++git) {
      depth_map_region_sptr gp_reg = *git;
      volm_spherical_query_region sph_reg;
      sph_reg.set_from_depth_map_region(cam, gp_reg, sph_vol_);
      sph_regions_[roll_idx].push_back(sph_reg);
    }

    vcl_vector<depth_map_region_sptr> object_regions =
      dm_scene_->scene_regions();
    unsigned n_regions = object_regions.size();
    for (unsigned reg_idx = 0; reg_idx<n_regions; ++reg_idx) {
      depth_map_region_sptr obj_reg = object_regions[reg_idx];
      volm_spherical_query_region sph_reg;
      sph_reg.set_from_depth_map_region(cam, obj_reg, sph_vol_);
      sph_regions_[roll_idx].push_back(sph_reg);
    }
  }
}

vcl_vector<volm_spherical_query_region> volm_spherical_region_query::
query_regions(unsigned roll_indx)
{
  return sph_regions_[roll_indx];
}

void volm_spherical_region_query::print(vcl_ostream& os) const
{
  vcl_map< unsigned, vcl_vector<volm_spherical_query_region> >::const_iterator rolit =  sph_regions_.begin();
  for (; rolit != sph_regions_.end(); ++rolit) {
    vcl_cout << "BoundingBoxes for roll = " << cam_space_->roll(rolit->first)
             << " degrees\n";
    const vcl_vector<volm_spherical_query_region>& sr = rolit->second;
    for (vcl_vector<volm_spherical_query_region>::const_iterator rit=sr.begin();
         rit != sr.end(); ++rit) {
      rit->print(os);
    }
    vcl_cout << '\n';
  }
}

void volm_spherical_region_query::
display_query_regions(vsph_unit_sphere_sptr const& usph_ptr,
                      vcl_string const& path, unsigned roll_index)
{
  vcl_vector<float> ndef(3, 0.5f);
  vcl_vector<vcl_vector<float> > reg_color(usph_ptr->size(), ndef);
  vcl_vector<volm_spherical_query_region>& qrs = sph_regions_[roll_index];
  unsigned n = qrs.size();
  vsph_unit_sphere::const_iterator sit = usph_ptr->begin();
  unsigned idx = 0;
  for (; sit != usph_ptr->end(); ++sit, ++idx) {
    const vsph_sph_point_2d & sp = *sit;

    for (unsigned i = 0; i<n; ++i) {
      volm_spherical_query_region& qr = qrs[i];
      if (qr.inside(sp)) {
        unsigned char ocode = qr.orientation();
        vcl_vector<float> c = depth_map_region::orient_color(ocode);
        reg_color[idx]= c;
      }
    }
  }
  usph_ptr->display_color(path, reg_color, ndef);
}
