#include "volm_desc_land.h"
#include <volm/volm_io.h>
#include <vcl_algorithm.h>
#include <volm/volm_category_io.h>

#if 0
unsigned obtain_bin_size() {
  return volm_osm_category_io::volm_land_table.size();
}

unsigned volm_desc_land::n_bins = obtain_bin_size();
#endif

// Constructor - returns a descriptor with a histogram of all bins 0 if it cannot find the land type, e.g. pass -1 to get such a histogram
volm_desc_land::volm_desc_land(int land_type_id) 
{
  name_ = "volm_desc_land";
  nbins_ = volm_osm_category_io::volm_land_table.size();
  h_.resize(nbins_, (unsigned char)0);

  if (land_type_id > 0) {
    // current land_type_id is from geo cover, using volm_osm_category_io 
    vcl_map<int, volm_land_layer>::iterator mit = volm_osm_category_io::geo_land_table.find(land_type_id);
    if (mit != volm_osm_category_io::geo_land_table.end())
      h_[mit->second.id_] = 1;
  }
  //if (land_type_id > 0) { // we don't want to consider invalid = 0 as one of the land types and leave the histogram as all zeros 
  //  vcl_map<int, volm_attributes >::iterator it = volm_label_table::land_id.find(land_type_id);
  //  if (it != volm_label_table::land_id.end())
  //    h_[it->second.id_ - 1] = 1;
  //}
}

vcl_string trim(const vcl_string & s)
{
  vcl_size_t start = s.find_first_not_of(" ");
  if (start == vcl_string::npos) return "";
  unsigned end = s.find_last_not_of(" ");
  return s.substr(start, 1+end-start);
}

// Constructor for the query
volm_desc_land::volm_desc_land(vcl_string& filename)
{
  name_ = "volm_desc_land";
  nbins_ = volm_osm_category_io::volm_land_table.size();
  h_.resize(nbins_, (unsigned char)0);

  // read the file and determine the land type of the camera location of the query
  vcl_ifstream ifs(filename.c_str());
  char buffer[1000];
  ifs.getline(buffer, 1000);
  vcl_string cat_name(buffer); // ifs >> cat_name;

  // now search in the land_type table in volm_io
  int cat_id;
  vcl_map<vcl_string,  volm_land_layer>::iterator mit = volm_osm_category_io::volm_land_table_name.find(cat_name);
  if (mit != volm_osm_category_io::volm_land_table_name.end())
    h_[mit->second.id_] = 1;
#if 0
  for (vcl_map<int, volm_attributes >::iterator it = volm_label_table::land_id.begin(); it != volm_label_table::land_id.end(); it++) {
    if (it->second.contains(cat_name)) {
      NLCD_id = it->first;
      cat_id = it->second.id_;
      break;
    }
  }
  vcl_cout << " image category: " << cat_name << " NLCD id: " << NLCD_id << " category id: " << cat_id;
  h_[cat_id-1] = 1;
#endif
}

float volm_desc_land::similarity(volm_desc_sptr other)
{
  if (nbins_ != other->nbins())
    return 0.0f;
  // calcualte the inersection
  float intersec = 0.0f;
  for (unsigned idx = 0; idx < nbins_; idx++) {
    intersec += (float)vcl_min(this->count(idx), other->count(idx));
  }
  return intersec;  // intersec is 0 or 1
}