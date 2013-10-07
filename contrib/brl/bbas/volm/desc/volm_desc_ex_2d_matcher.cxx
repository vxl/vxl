#include "volm_desc_ex_2d_matcher.h"
#include <vcl_algorithm.h>
//
// \file

float volm_desc_ex_2d_matcher::score(volm_desc_sptr const& a, volm_desc_sptr const& b)
{
  return a->similarity(b);
}

volm_desc_sptr volm_desc_ex_2d_matcher::create_query_desc()
{
  if (radius_.empty())
    radius_.push_back(1.0);
  // sort the radius to ensure the bin order
  vcl_sort(radius_.begin(), radius_.end());
  unsigned ndists = (unsigned)radius_.size() + 1;
  
  double largest_rad = radius_[radius_.size()-1];  // this is in meters


  // create the ex_land_only descriptor
  volm_desc_ex_land_only* desc = new volm_desc_ex_land_only(ndists, nlands_, radius_);

  // loop over the depth map scene, note the sky is completely ignored
  // ground (expend it to all rings)
  vcl_vector<double> radius = desc->radius();
  if (!dms_->ground_plane().empty()) {
    vcl_vector<depth_map_region_sptr> grd = dms_->ground_plane();
    for (unsigned g_idx = 0; g_idx < grd.size(); g_idx++) {
      if (!grd[g_idx]->active())
        continue;
      if (grd[g_idx]->min_depth() > largest_rad)
        continue;
      desc->set_count(grd[g_idx]->min_depth(), grd[g_idx]->land_id(), (unsigned char)1);
      for (unsigned i = 0; i < radius.size(); i++) {
        double grd_dst = grd[g_idx]->min_depth()+radius[i]+1;
        if (grd_dst < largest_rad)
          desc->set_count(grd_dst, grd[g_idx]->land_id(), (unsigned char)1);;
      }
    }
  }
  // other objects
  if (!dms_->scene_regions().empty()) {
    vcl_vector<depth_map_region_sptr> obj = dms_->scene_regions();
    for (unsigned o_idx = 0; o_idx < obj.size(); o_idx++) {
      if (!obj[o_idx]->active())
        continue;
      if (obj[o_idx]->min_depth() > largest_rad)
        continue;
      desc->set_count(obj[o_idx]->min_depth(), obj[o_idx]->land_id(), (unsigned char)1);
    }
  }
  volm_desc_sptr query(desc);
  return query;
}

bool volm_desc_ex_2d_matcher::check_threshold(volm_desc_sptr const& query, float& thres_value)
{
  unsigned num_valid_bin = query->get_area();
  if (thres_value < 1.0f/num_valid_bin)
    thres_value = 1.0f/num_valid_bin;
  return true;
}