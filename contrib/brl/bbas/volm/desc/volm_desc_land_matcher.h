#pragma once
// This is brl/bbas/volm/desc/volm_desc_land_matcher.h
#ifndef volm_desc_land_matcher_h_
#define volm_desc_land_matcher_h_
//:
// \file
// \brief  A class to match query land descriptor to existance indices
//
// \author Yi Dong
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include "volm_desc_matcher.h"
#include "volm_desc_land_indexer.h"
#include "volm_desc_land.h"

class volm_desc_land_matcher : public volm_desc_matcher
{
public:
  // Default constructor
  volm_desc_land_matcher() = default;

  //: Constructor
  volm_desc_land_matcher(std::string const& NLCD_folder, vgl_point_3d<double>& query_gt_loc);

  //: Destrcutor
  ~volm_desc_land_matcher() override = default;

  //: Create a volumetric land descriptor for the query image
  volm_desc_sptr create_query_desc() override;

  //: Compare two descriptor a and b and return a score to quantifies the similarity
  float score(volm_desc_sptr const& query, volm_desc_sptr const& index) override;

  std::string get_index_type_str() override { return volm_desc_land_indexer::get_name(); }

private:
  //std::string query_category_file_;
  std::vector<volm_img_info> NLCD_imgs_;
  vgl_point_3d<double> query_gt_loc_;

};

#endif  // volm_desc_land_matcher_h_
