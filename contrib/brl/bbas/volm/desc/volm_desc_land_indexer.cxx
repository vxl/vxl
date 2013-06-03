#include "volm_desc_land_indexer.h"

volm_desc_land_indexer::volm_desc_land_indexer(vcl_string const& NLCD_folder, vcl_string const& out_index_folder) : volm_desc_indexer(out_index_folder)
{
  //volm_io_tools::load_nlcd_imgs(NLCD_folder, NLCD_imgs_);
}

bool volm_desc_land_indexer::extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values)
{
  // use location to read the land type from NLCD image
  // TODO.. 

  // for now just dummy type 
  volm_desc_sptr desc = new volm_desc_land(11);  // pass water for now
  desc->get_char_array(values);
  return true;
}

