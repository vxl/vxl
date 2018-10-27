#pragma once
// This is brl/bseg/boxm2/volm/desc/boxm2_volm_desc_ex_land_only_matcher.h
#ifndef boxm2_volm_desc_ex_land_only_matcher_h_
#define boxm2_volm_desc_ex_land_only_matcher_h_
//:
// \file
// \brief  A class to match query existance land only descriptor to existance land only indices
//
// \author Yi Dong
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <utility>
#include <volm/desc/volm_desc_matcher.h>
#include <volm/desc/volm_desc_ex_land_only.h>
#include <volm/desc/volm_desc_indexer.h>

#include "boxm2_volm_desc_ex_land_only_indexer.h"

class boxm2_volm_desc_ex_land_only_matcher : public volm_desc_matcher
{
public:
  //: Default constructor
  boxm2_volm_desc_ex_land_only_matcher() = default;

  //: Constructor
  boxm2_volm_desc_ex_land_only_matcher(depth_map_scene_sptr const& dms,
                                       std::vector<double>  radius,
                                       unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                                       unsigned char const& initial_mag = 0)
  : dms_(dms), radius_(std::move(radius)), nlands_(nlands), initial_mag_(initial_mag)
  {}

  //: Destructor
  ~boxm2_volm_desc_ex_land_only_matcher() override = default;

  //: check given threshold is valid or not for generate scaled probability map
  bool check_threshold(volm_desc_sptr const& query, float& thres_value) override;

  //: Compare two descriptor a and b using the similarity method implemented in descriptor a
  float score(volm_desc_sptr const& query, volm_desc_sptr const& index) override;

  //: Create a volumetric existence descriptor for the query image
  volm_desc_sptr create_query_desc() override;

  std::string get_index_type_str() override { return volm_desc_ex_land_only_indexer::get_name(); }

private:
    //: query depth_map_scene
  depth_map_scene_sptr  dms_;
  //: parameters related to volumetric existence descriptor
  std::vector<double> radius_;
  unsigned           nlands_;
  unsigned char initial_mag_;
};

#endif // boxm2_volm_desc_ex_land_only_matcher_h_
