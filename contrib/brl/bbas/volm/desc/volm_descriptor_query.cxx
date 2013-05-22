// This is brl/bbas/volm/desc/volm_descriptor_query.cxx
#include "volm_descriptor_query.h"

volm_descriptor_query::volm_descriptor_query(depth_map_scene_sptr const& dm,
                                             vcl_vector<double> const& radius,
                                             vcl_vector<double> const& height,
                                             unsigned const& n_orient,
                                             unsigned const& n_land,
                                             unsigned char const& initial_mag)
{
  dm_ = dm;
  vd_ = new volm_descriptor(radius, height, n_orient, n_land, initial_mag);
  // construct the histogram from depth_map_scene
  // Note all the possible land types defind in fallback category 
  // sky
  if (!dm_->sky().empty()) {
    vcl_vector<depth_map_region_sptr> sky = dm_->sky();
    for (unsigned s_idx = 0; s_idx < sky.size(); s_idx++)
      vd_->set_count(sky[s_idx]->min_depth(), 1E6, sky[s_idx]->orient_type(), sky[s_idx]->land_id(), (unsigned char)1);
  }
  // ground
  if (!dm_->ground_plane().empty()) {
    vcl_vector<depth_map_region_sptr> grd = dm_->ground_plane();
    for (unsigned g_idx = 0; g_idx < grd.size(); g_idx++) {
      vcl_vector<unsigned char> land_fallback_id = volm_fallback_label::fallback_id[grd[g_idx]->land_id()];
      for (unsigned f_idx = 0; f_idx < land_fallback_id.size(); f_idx++)
        vd_->set_count(grd[g_idx]->min_depth(), 1E6, grd[g_idx]->orient_type(), land_fallback_id[f_idx], (unsigned char)1);
    }
  }
  // non sky/ground object types
  if (!dm_->scene_regions().empty()) {
    vcl_vector<depth_map_region_sptr> obj = dm_->scene_regions();
    for (unsigned o_idx = 0; o_idx < obj.size(); o_idx++) {
      vcl_vector<unsigned char> land_fallback_id = volm_fallback_label::fallback_id[obj[o_idx]->land_id()];
      for (unsigned f_idx = 0; f_idx < land_fallback_id.size(); f_idx++)
        vd_->set_count(obj[o_idx]->min_depth(), 1E6, obj[o_idx]->orient_type(), land_fallback_id[f_idx], (unsigned char)1);
    }
  }
}

void volm_descriptor_query::print() const
{
  vcl_cout << " ===== query region infomation: =====\n";
  if (!dm_->sky().empty()) {
    vcl_cout << "\t SKY regions: \n";
    vcl_vector<depth_map_region_sptr> sky = dm_->sky();
    for (unsigned s_idx = 0; s_idx < sky.size(); s_idx++)
      vcl_cout << "name = " << sky[s_idx]->name() << ",depth = " << sky[s_idx]->min_depth()
               << ",orient = " << sky[s_idx]->orient_string(sky[s_idx]->orient_type())
               << ",land = " << volm_label_table::land_string(sky[s_idx]->land_id()) << '\n';
  }
  if (!dm_->ground_plane().empty()) {
    vcl_cout << "\t Ground Plane regions: \n";
    vcl_vector<depth_map_region_sptr> grd = dm_->ground_plane();
    for (unsigned g_idx = 0; g_idx < grd.size(); g_idx++) {
      vcl_cout << "name = " << grd[g_idx]->name() << ",depth = " << grd[g_idx]->min_depth()
               << ",orient = " << grd[g_idx]->orient_string(grd[g_idx]->orient_type())
               << ",land = " << volm_label_table::land_string(grd[g_idx]->land_id()) << " -->  = ";
      volm_fallback_label::print_id(grd[g_idx]->land_id());
      vcl_cout << '\n';
    }
  }
  if (!dm_->scene_regions().empty()) {
    vcl_cout << "\t Other regions: \n";
    vcl_vector<depth_map_region_sptr> obj = dm_->scene_regions();
    for (unsigned o_idx = 0; o_idx < obj.size(); o_idx++) {
      vcl_cout << "name = " << obj[o_idx]->name() << ",depth = " << obj[o_idx]->min_depth()
               << ",orient = " << obj[o_idx]->orient_string(obj[o_idx]->orient_type())
               << ",land = " << volm_label_table::land_string(obj[o_idx]->land_id()) << " -->  = ";
      volm_fallback_label::print_id(obj[o_idx]->land_id());
      vcl_cout << '\n';
    }
  }
  vcl_cout << " ====== transferred histrogram descriptor ======\n";
  vd_->print();
}