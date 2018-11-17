#include <iostream>
#include <algorithm>
#include "volm_desc_land.h"
#include <volm/volm_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_category_io.h>

#if 0
unsigned obtain_bin_size() {
  return volm_osm_category_io::volm_land_table.size();
}

unsigned volm_desc_land::n_bins = obtain_bin_size();
#endif

// Constructor - returns a descriptor with a histogram of all bins 0 if it cannot find the land type, e.g. pass -1 to get such a histogram
volm_desc_land::volm_desc_land(int land_type_id, std::string const& id_type)
{
  name_ = "volm_desc_land";
  nbins_ = volm_osm_category_io::volm_land_table.size();
  h_.resize(nbins_, (unsigned char)0);
  if (land_type_id > 0) {  // we don't want to consider invalid = 0 as one of the land types and leave the histogram as all zeros
    if (id_type.compare("geo_cover") == 0) {
      // current land_type_id is from geo cover, using volm_osm_category_io  (for phase1b)
      auto mit = volm_osm_category_io::geo_land_table.find(land_type_id);
      if (mit != volm_osm_category_io::geo_land_table.end())
        h_[mit->second.id_] = 1;
    }
    else if (id_type.compare("NLCD") == 0) {
      // use NLCD as input data
      auto mit = volm_osm_category_io::nlcd_land_table.find(land_type_id);
      if (mit != volm_osm_category_io::nlcd_land_table.end())
        h_[mit->second.id_] = 1;
    }
    else if (id_type.compare("volm") == 0) {
      // input land_type_id is the id defined in volm_osm_category_io
      auto mit = volm_osm_category_io::volm_land_table.find(land_type_id);
      if (mit != volm_osm_category_io::volm_land_table.end())
        h_[mit->second.id_] = 1;
    }
  }
}

std::string trim(const std::string & s)
{
  std::size_t start = s.find_first_not_of(' ');
  if (start == std::string::npos) return "";
  unsigned end = s.find_last_not_of(' ');
  return s.substr(start, 1+end-start);
}

// Constructor for the query
volm_desc_land::volm_desc_land(std::string& filename)
{
  name_ = "volm_desc_land";
  nbins_ = volm_osm_category_io::volm_land_table.size();
  h_.resize(nbins_, (unsigned char)0);

  // read the file and determine the land type of the camera location of the query
  std::ifstream ifs(filename.c_str());
  char buffer[1000];
  ifs.getline(buffer, 1000);
  std::string cat_name(buffer); // ifs >> cat_name;

  // now search in the land_type table in volm_io
  auto mit = volm_osm_category_io::volm_land_table_name.find(cat_name);
  if (mit != volm_osm_category_io::volm_land_table_name.end())
    h_[mit->second.id_] = 1;
#if 0
  for (std::map<int, volm_attributes >::iterator it = volm_label_table::land_id.begin(); it != volm_label_table::land_id.end(); it++) {
    if (it->second.contains(cat_name)) {
      NLCD_id = it->first;
      cat_id = it->second.id_;
      break;
    }
  }
  std::cout << " image category: " << cat_name << " NLCD id: " << NLCD_id << " category id: " << cat_id;
  h_[cat_id-1] = 1;
#endif
}

float volm_desc_land::similarity(volm_desc_sptr other)
{
  if (nbins_ != other->nbins())
    return 0.0f;
  // calculate the intersection
  float intersec = 0.0f;
  for (unsigned idx = 0; idx < nbins_; idx++) {
    intersec += (float)std::min(this->count(idx), other->count(idx));
  }
  return intersec;  // intersect is 0 or 1
}
