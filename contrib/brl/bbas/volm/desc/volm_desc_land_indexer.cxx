#include "volm_desc_land_indexer.h"

vcl_string volm_desc_land_indexer::name_ = "land";

volm_desc_land_indexer::volm_desc_land_indexer(vcl_string const& NLCD_folder, vcl_string const& out_index_folder) : volm_desc_indexer(out_index_folder)
{
  volm_io_tools::load_nlcd_imgs(NLCD_folder, NLCD_imgs_);
}

bool volm_desc_land_indexer::extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values)
{
  // use location to read the land type from NLCD image
  bool found_it = false;
  volm_desc_sptr desc;
  for (unsigned i = 0; i < NLCD_imgs_.size(); i++) {
    if (NLCD_imgs_[i].bbox.contains(lon, lat)) {
      vil_image_view<vxl_byte> img(NLCD_imgs_[i].img_r);

      // get the land type of the location
      double u, v;
      NLCD_imgs_[i].cam->global_to_img(-lon, lat, elev, u, v);
      unsigned uu = (unsigned)vcl_floor(u + 0.5);
      unsigned vv = (unsigned)vcl_floor(v + 0.5);
      if (uu > 0 && vv > 0 && uu < NLCD_imgs_[i].ni && vv < NLCD_imgs_[i].nj) {
        unsigned char label = img(uu, vv);
        desc = new volm_desc_land(label);
        found_it = true;
        break;
      }
    }   
  }

  if (!found_it) 
    desc = new volm_desc_land(-1);  // pass -1 to get a histogram of all bins 0  
  
  desc->get_char_array(values);
  return true;
}

