#include "volm_desc_land_indexer.h"



volm_desc_land_indexer::volm_desc_land_indexer(std::string const& NLCD_folder, std::string const& out_index_folder) : volm_desc_indexer(out_index_folder)
{
  //volm_io_tools::load_nlcd_imgs(NLCD_folder, NLCD_imgs_);
  volm_io_tools::load_imgs(NLCD_folder, NLCD_imgs_, true, true, true);
}

bool volm_desc_land_indexer::extract(double lat, double lon, double elev, std::vector<unsigned char>& values)
{
  // use location to read the land type from NLCD image
  unsigned char label = 0;
  bool found_it = volm_io_tools::get_location_nlcd(NLCD_imgs_, lat, lon, elev, label);
  volm_desc_sptr desc;
  if (found_it)
    desc = new volm_desc_land(label);
  else
    desc = new volm_desc_land(-1);  // pass -1 to get a histogram of all bins 0

  desc->get_char_array(values);
  return true;
}
