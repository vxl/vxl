#include "volm_spherical_region_query.h"
//
#include <vsph/vsph_unit_sphere.h>
#include <bsol/bsol_algs.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vsph/vsph_utils.h>
#include <depth_map/depth_map_region.h>
#include <depth_map/depth_map_scene.h>
#include <volm/volm_char_codes.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_region.h>

volm_spherical_region_query::volm_spherical_region_query(depth_map_scene_sptr const& dm_scene,
                                                         volm_camera_space_sptr const& cam_space,
                                                         volm_spherical_container_sptr const& sph_vol)
                                                         : dm_scene_(dm_scene), cam_space_(cam_space), sph_vol_(sph_vol)
{
    canonical_top_fov_ = cam_space_->top_fov(0);
    canonical_head_    =cam_space_->head_mid();
    canonical_tilt_    = cam_space_->tilt_mid();
    this->construct_spherical_regions();
}

volm_spherical_region volm_spherical_region_query::set_from_depth_map_region(vpgl_perspective_camera<double> const& cam, depth_map_region_sptr const& dm_region)
{
    // form the bounding box
    vsph_sph_box_2d bbox;
    vgl_polygon<double> poly = bsol_algs::vgl_from_poly(dm_region->region_2d());
    vgl_polygon<double> sph_poly =vsph_utils::project_poly_onto_unit_sphere(cam, poly);
    unsigned n_sheets = sph_poly.num_sheets();
    for (unsigned sh_idx = 0; sh_idx<n_sheets; ++sh_idx) {
        unsigned n_verts = sph_poly[sh_idx].size();
        for (unsigned v_idx = 0; v_idx < n_verts; ++v_idx) {
            vgl_point_2d<double> pt = sph_poly[sh_idx][v_idx];
            vsph_sph_point_2d sph_pt(pt.x(), pt.y());
            bbox.add(sph_pt);
        }
    }
    volm_spherical_region r(bbox);
    //extract object attributes
    depth_map_region::orientation otype = dm_region->orient_type();
    unsigned char min_depth = static_cast<unsigned char>(dm_region->min_depth());
    unsigned char max_depth = static_cast<unsigned char>(dm_region->max_depth());
#if 0 // unused ...
    unsigned char order  =  static_cast<unsigned char>(dm_region->order());
    unsigned char nlcd_id = static_cast<unsigned char>(dm_region->land_id());
#endif
    vcl_string nam = dm_region->name();
    if (otype == depth_map_region::GROUND_PLANE) {
        r.set_attribute(ORIENTATION, static_cast<unsigned char>(depth_map_region::HORIZONTAL));
        r.set_attribute(MIN_DEPTH, static_cast<unsigned char>(sph_vol_->min_voxel_res()));
        r.set_attribute(MAX_DEPTH, static_cast<unsigned char>(sph_vol_->get_depth_interval(max_depth)));
    }
    if (otype == depth_map_region::INFINT||nam=="sky") {
        r.set_attribute(ORIENTATION, static_cast<unsigned char>(depth_map_region::INFINT));
        r.set_attribute(MIN_DEPTH, sky_depth);
        r.set_attribute(MAX_DEPTH, sky_depth);
        r.set_attribute(DEPTH_ORDER, sky_order);
    }
    else {
        r.set_attribute(ORIENTATION, static_cast<unsigned char>(otype));
        r.set_attribute(MIN_DEPTH, min_depth);
        r.set_attribute(MAX_DEPTH, max_depth);
    }
    return r;
}

void volm_spherical_region_query::construct_spherical_regions()
{
    unsigned n_roll = cam_space_->n_roll();
    unsigned roll_idx = 0;
    for ( roll_idx = 0; roll_idx < n_roll; ++roll_idx) {
        double roll = cam_space_->roll(roll_idx);
        cam_angles cangs(roll, canonical_top_fov_, canonical_head_, canonical_tilt_);
        cangs.print();
        // camera at indicated roll and other rotations nominal.
        int cam_index = cam_space_->closest_index(cangs);
        assert(cam_index>=0);
        vpgl_perspective_camera<double> cam = cam_space_->camera(cam_index);
        // ====  construct object regions =====
        vcl_vector<depth_map_region_sptr> sky_regions =dm_scene_->sky();
        for (vcl_vector<depth_map_region_sptr>::iterator sit = sky_regions.begin();
             sit != sky_regions.end(); ++sit) {
            volm_spherical_region sph_reg = this->set_from_depth_map_region(cam, *sit);
            sph_regions_[roll_idx].add_region(sph_reg);
        }
        vcl_vector<depth_map_region_sptr> gp_regions = dm_scene_->ground_plane();
        for (vcl_vector<depth_map_region_sptr>::iterator git = gp_regions.begin();git != gp_regions.end(); ++git) {
            volm_spherical_region sph_reg = this->set_from_depth_map_region(cam, *git);
            sph_regions_[roll_idx].add_region(sph_reg);
        }
        vcl_vector<depth_map_region_sptr> object_regions =dm_scene_->scene_regions();
        unsigned n_regions = object_regions.size();
        for (unsigned reg_idx = 0; reg_idx<n_regions; ++reg_idx) {
            volm_spherical_region sph_reg = this->set_from_depth_map_region(cam, object_regions[reg_idx]);
            sph_regions_[roll_idx].add_region(sph_reg);
        }
    }
}

volm_spherical_regions_layer volm_spherical_region_query::query_regions(unsigned roll_indx)
{
    return sph_regions_[roll_indx];
}

void volm_spherical_region_query::print(vcl_ostream& os) const
{
    vcl_map< unsigned, volm_spherical_regions_layer >::const_iterator rolit =  sph_regions_.begin();
    for (; rolit != sph_regions_.end(); ++rolit) {
        vcl_cout << "BoundingBoxes for roll = " << cam_space_->roll(rolit->first)<< " degrees\n";
        volm_spherical_regions_layer  region_layer = rolit->second;
        vcl_vector<volm_spherical_region> regions = region_layer.regions();
        for (unsigned i = 0 ; i < regions.size(); i++) {
            regions[i].print(os);
        }
        vcl_cout << '\n';
    }
}

