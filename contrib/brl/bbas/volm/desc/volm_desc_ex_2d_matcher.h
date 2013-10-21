#pragma once
// This is brl/bbas/volm/desc/volm_desc_ex_2d_matcher.h
#ifndef volm_desc_ex_2d_matcher_h_
#define volm_desc_ex_2d_matcher_h_
//:
// \file
// \brief  A class to match query 2D existance descriptor to 2D existance indices  
//         Note anything further than 1000 m is completely ignored and the query is described by a volm_desc_ex_land_only descriptor
//
// \author Yi Dong
// \date July 01, 2013
// \verbatim
//  Modifications
//   October 08, 2013  Yi Dong -- add a float formatted histogram to store weight parameters
// \endverbatim
//

#include "volm_desc_matcher.h"
#include "volm_desc_ex_land_only.h"
#include "volm_desc_indexer.h"
#include "volm_desc_ex_2d_indexer.h"
#include <volm/volm_category_io.h>

class volm_desc_ex_2d_matcher : public volm_desc_matcher
{
public:
  //: Default constructor
  volm_desc_ex_2d_matcher() {}

  //: Constructor
  volm_desc_ex_2d_matcher(depth_map_scene_sptr const& dms,
                          vcl_vector<volm_weight> const& weights,
                          vcl_vector<double> const& radius,
                          unsigned const& nlands = volm_osm_category_io::volm_land_table.size(),
                          unsigned char const& initial_mag = 0)
                          : dms_(dms), weights_(weights), radius_(radius), nlands_(nlands), initial_mag_(initial_mag) {}

  // Destructor
  ~volm_desc_ex_2d_matcher() {}

  //: check given threshold is valid or not for generate scaled probability map
  virtual bool check_threshold(volm_desc_sptr const& query, float& thres_value);

  //: Compare two descriptor a and b using the similarity method implemented in descriptor a
  virtual float score(volm_desc_sptr const& query, volm_desc_sptr const& index);

  //: Create a volumetric existance descriptor for the query image
  virtual volm_desc_sptr create_query_desc();

  virtual vcl_string get_index_type_str() { return volm_desc_ex_2d_indexer::name_; }

private:
    //: query depth_map_scene
  depth_map_scene_sptr  dms_;
  //volm_desc_sptr      query_;
  
  //: parameters related to volumetric existance descriptor
  vcl_vector<double> radius_;
  unsigned           nlands_;
  unsigned char initial_mag_;
  
  //: weight parameters
  vcl_vector<volm_weight> weights_;
  vcl_vector<float> weights_hist_;

  //: function to find weight value given the name of the region
  float find_wgt_value(vcl_string const& name);
};

#endif // volm_desc_ex_2d_matcher_h_