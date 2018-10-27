#pragma once
// This is brl/bbas/volm/desc/volm_desc_ex_2d_matcher.h
#ifndef volm_desc_ex_2d_matcher_h_
#define volm_desc_ex_2d_matcher_h_
//:
// \file
// \brief  A class to match query 2D existence descriptor to 2D existance indices
//         Note anything further than 1000 m is completely ignored and the query is described by a volm_desc_ex_land_only descriptor
//
// \author Yi Dong
// \date July 01, 2013
// \verbatim
//  Modifications
//   October 08, 2013  Yi Dong -- add a float formatted histogram to store weight parameters
// \endverbatim
//

#include <utility>
#include "volm_desc_matcher.h"
#include "volm_desc_ex_land_only.h"
#include "volm_desc_indexer.h"
#include "volm_desc_ex_2d_indexer.h"
#include <volm/volm_category_io.h>


class volm_desc_ex_2d_matcher : public volm_desc_matcher
{
public:
  //: Default constructor
  volm_desc_ex_2d_matcher() = default;

  //: Constructor
  volm_desc_ex_2d_matcher(depth_map_scene_sptr const& dms,
                          std::vector<volm_weight>  weights,
                          std::vector<double>  radius,
                          unsigned const& nlands = volm_osm_category_io::volm_land_table.size(),
                          unsigned char const& initial_mag = 0)
                          : dms_(dms), radius_(std::move(radius)), nlands_(nlands), initial_mag_(initial_mag), weights_(std::move(weights)) {}

  // Destructor
  ~volm_desc_ex_2d_matcher() override = default;

  //: check given threshold is valid or not for generate scaled probability map
  bool check_threshold(volm_desc_sptr const& query, float& thres_value) override;

  //: Compare two descriptor a and b using the similarity method implemented in descriptor a
  float score(volm_desc_sptr const& query, volm_desc_sptr const& index) override;

  //: Create a volumetric existence descriptor for the query image
  volm_desc_sptr create_query_desc() override;

  std::string get_index_type_str() override { return volm_desc_ex_2d_indexer::get_name(); }

private:
    //: query depth_map_scene
  depth_map_scene_sptr  dms_;
  //volm_desc_sptr      query_;

  //: parameters related to volumetric existence descriptor
  std::vector<double> radius_;
  unsigned           nlands_;
  unsigned char initial_mag_;

  //: weight parameters
  std::vector<volm_weight> weights_;
  std::vector<float> weights_hist_;

  //: function to find weight value given the name of the region
  float find_wgt_value(std::string const& name);
};

#endif // volm_desc_ex_2d_matcher_h_
