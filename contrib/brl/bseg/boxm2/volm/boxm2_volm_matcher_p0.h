// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p0.h
#ifndef boxm2_volm_matcher_p0_h_
#define boxm2_volm_matcher_p0_h_


#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_spherical_region_index.h>
#include <volm/volm_spherical_region_query.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_io.h>

#include "boxm2_volm_wr3db_index_sptr.h"
#include "boxm2_volm_wr3db_index.h"

class boxm2_volm_matcher_p0
{
 public:
    ~boxm2_volm_matcher_p0();
    boxm2_volm_matcher_p0(const volm_camera_space_sptr& cam_space,volm_spherical_region_query srq,float threshold);
    // matcher function
    bool match(volm_spherical_region_index & index, const volm_score_sptr& score);

    bool match_order(volm_spherical_regions_layer & index_layer, double & score, volm_spherical_regions_layer & q_regions, camera_space_iterator & iter, vsph_sph_box_2d & imbox);
    long count_ ;
 private:

    volm_camera_space_sptr cam_space_;
    volm_spherical_region_query query_;
    float threshold_;
    unsigned int max_cam_per_loc_;

    std::map<unsigned int, std::vector<vsph_sph_box_2d> > query_xformed_boxes_;
    std::map<unsigned int,vsph_sph_box_2d > imbox_xformed_;
};

#endif
