#include "volm_desc_land.h"
#include <volm/volm_io.h>

unsigned volm_desc_land::n_bins = 14;

// Constructor - returns a descriptor with a histogram of all bins 0 if it cannot find the land type, e.g. pass -1 to get such a histogram
volm_desc_land::volm_desc_land(int land_type_id) 
{
  name_ = "volm_desc_land";
  nbins_ = volm_desc_land::n_bins;
  h_.resize(nbins_, (unsigned char)0);

  if (land_type_id > 0) { // we don't want to consider invalid = 0 as one of the land types and leave the histogram as all zeros 
    vcl_map<int, volm_attributes >::iterator it = volm_label_table::land_id.find(land_type_id);
    if (it != volm_label_table::land_id.end())
      h_[it->second.id_ - 1] = 1;
  }
}

// Constructor for the query
volm_desc_land::volm_desc_land(vcl_string& filename)
{
  name_ = "volm_desc_land";
  nbins_ = volm_desc_land::n_bins;
  h_.resize(nbins_, (unsigned char)0);

  // read the file and determine the land type of the camera location of the query
  vcl_ifstream ifs(filename.c_str());
  vcl_string cat_name; ifs >> cat_name;

  // now search in the land_type table in volm_io
  int NLCD_id, cat_id;

  for (vcl_map<int, volm_attributes >::iterator it = volm_label_table::land_id.begin(); it != volm_label_table::land_id.end(); it++) {
    if (it->second.contains(cat_name)) {
      NLCD_id = it->first;
      cat_id = it->second.id_;
      break;
    }
  }
  vcl_cout << " image category: " << cat_name << " NLCD id: " << NLCD_id << " category id: " << cat_id;
  h_[cat_id-1] = 1;
}




