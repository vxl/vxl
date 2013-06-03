#include "volm_desc_land.h"
#include <volm/volm_io.h>

unsigned volm_desc_land::n_bins = 14;

// Constructor
volm_desc_land::volm_desc_land(int land_type_id) 
{
  name_ = "volm_desc_land";
  nbins_ = volm_desc_land::n_bins;
  h_.resize(nbins_);

  vcl_map<int, volm_attributes >::iterator it = volm_label_table::land_id.find(land_type_id);
  if (it != volm_label_table::land_id.end())
    h_[it->second.id_ - 1] = 1;
  else {
    vcl_cerr << " cannot find NLCD id: " << land_type_id << " in volm_land_table::land_id!!\n";
    throw 0;
  }
}

// Constructor for the query
volm_desc_land::volm_desc_land(vcl_string& filename)
{
  name_ = "volm_desc_land";
  nbins_ = volm_desc_land::n_bins;
  h_.resize(nbins_, (unsigned char)0);

  // read the file and determine the land type of the camera location of the query
  vcl_ifstream ifs(filename);
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




