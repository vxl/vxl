#include <iostream>
#include <vector>
#include "volm_spherical_region_query.h"
#include <vsph/vsph_unit_sphere.h>
#include <bsol/bsol_algs.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsph/vsph_utils.h>
#include <volm/volm_char_codes.h>
#include <depth_map/depth_map_region.h>
#include <depth_map/depth_map_scene.h>
#include <volm/volm_spherical_container.h>
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

volm_spherical_region
volm_spherical_region_query::set_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                                                       depth_map_region_sptr const& dm_region)
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
    double min_depth = dm_region->min_depth();
    double max_depth = dm_region->max_depth();
    auto order =  static_cast<unsigned char>(dm_region->order());
    auto nlcd_id = static_cast<unsigned char>(dm_region->land_id());
    std::string nam = dm_region->name();
    if (otype == depth_map_region::GROUND_PLANE) {

        r.set_attribute(ORIENTATION, static_cast<unsigned char>(depth_map_region::HORIZONTAL));
        r.set_attribute(NLCD, nlcd_id);
    }
    if (nam.find("sky") !=std::string::npos ){
        r.set_attribute(SKY, static_cast<unsigned char>(254));
    }
    else {
        r.set_attribute(NLCD, nlcd_id);
        auto qval = static_cast<unsigned char>(otype);
        if (qval >=1 && qval <=3)
            r.set_attribute(ORIENTATION, (unsigned char) 2);
        else if (qval == 0)
            r.set_attribute(ORIENTATION, 1);

        r.set_attribute(MIN_DEPTH, (unsigned char)min_depth);
        r.set_attribute(MAX_DEPTH, (unsigned char)max_depth);
        r.set_attribute(DEPTH_ORDER, order);
    }

    return r;
}

volm_spherical_region
volm_spherical_region_query::set_ground_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                                                              depth_map_region_sptr const& dm_region)
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
    auto nlcd_id = static_cast<unsigned char>(dm_region->land_id());

    r.set_attribute(ORIENTATION, static_cast<unsigned char>(depth_map_region::HORIZONTAL));
    r.set_attribute(NLCD, nlcd_id);
    r.set_attribute(GROUND, 1);

    return r;
}

volm_spherical_region
volm_spherical_region_query::set_sky_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                                                           depth_map_region_sptr const& dm_region)
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
    r.set_attribute(SKY, 1);

    return r;
}

void volm_spherical_region_query::construct_spherical_regions()
{
    unsigned n_roll = cam_space_->n_roll();
    unsigned roll_idx = 0;
    for ( roll_idx = 0; roll_idx < n_roll; ++roll_idx) {
        double roll =cam_space_->roll_mid() + cam_space_->roll(roll_idx);
        cam_angles cangs(roll, canonical_top_fov_, canonical_head_, canonical_tilt_);
        cangs.print();
        // camera at indicated roll and other rotations nominal.
        int cam_index = cam_space_->closest_index(cangs);
        assert(cam_index>=0);
        vpgl_perspective_camera<double> cam = cam_space_->camera(cam_index);
        // ====  construct object regions =====
        std::vector<depth_map_region_sptr> sky_regions =dm_scene_->sky();
        for (auto & sky_region : sky_regions) {
            volm_spherical_region sph_reg = this->set_sky_from_depth_map_region(cam, sky_region);
            sph_regions_[roll_idx].add_region(sph_reg);
        }
        std::vector<depth_map_region_sptr> gp_regions = dm_scene_->ground_plane();
        for (auto & gp_region : gp_regions) {
            volm_spherical_region sph_reg = this->set_ground_from_depth_map_region(cam, gp_region);
            sph_regions_[roll_idx].add_region(sph_reg);
        }
        std::vector<depth_map_region_sptr> object_regions =dm_scene_->scene_regions();
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

void volm_spherical_region_query::print(std::ostream& os) const
{
    auto rolit =  sph_regions_.begin();
    for (; rolit != sph_regions_.end(); ++rolit) {
        std::cout << "BoundingBoxes for roll = " << cam_space_->roll(rolit->first)<< " degrees\n";
        volm_spherical_regions_layer  region_layer = rolit->second;
        std::vector<volm_spherical_region> regions = region_layer.regions();
        for (auto & region : regions) {
            region.print(os);
        }
        std::cout << '\n';
    }
}
