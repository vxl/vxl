#include "volm_desc_land_matcher.h"
//:
// \file

volm_desc_land_matcher::volm_desc_land_matcher(std::string const& NLCD_folder, vgl_point_3d<double>& query_gt_loc) : query_gt_loc_(query_gt_loc)
{
  //volm_io_tools::load_nlcd_imgs(NLCD_folder, NLCD_imgs_);
  volm_io_tools::load_imgs(NLCD_folder, NLCD_imgs_, true, true, true);
}

float volm_desc_land_matcher::score(volm_desc_sptr const& query, volm_desc_sptr const& index)
{
  return query->similarity(index);
}

volm_desc_sptr volm_desc_land_matcher::create_query_desc()
{
  //volm_desc_sptr query_land = new volm_desc_land(query_category_file_);
  volm_desc_sptr query_land = nullptr;
  unsigned char label;
  if (volm_io_tools::get_location_nlcd(NLCD_imgs_, query_gt_loc_.y(), query_gt_loc_.x(), query_gt_loc_.z(), label))
    query_land = new volm_desc_land((int)label);
  return query_land;
}
