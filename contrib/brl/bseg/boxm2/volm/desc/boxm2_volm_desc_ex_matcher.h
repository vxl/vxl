#pragma once
// This is brl/bseg/boxm2/volm/desc/boxm2_volm_desc_ex_matcher.h
#ifndef boxm2_volm_desc_ex_matcher_h_
#define boxm2_volm_desc_ex_matcher_h_
//:
// \file
// \brief  A class to match query existance descriptor to existance indices
//
// \author Yi Dong
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <volm/desc/volm_desc_matcher.h>
#include <volm/desc/volm_desc_ex.h>
#include <volm/desc/volm_desc_indexer.h>
#include "boxm2_volm_desc_ex_indexer.h"

class boxm2_volm_desc_ex_matcher : public volm_desc_matcher
{
  public:
  //: Defaule constructor
  boxm2_volm_desc_ex_matcher() {}

  //: Constructor
  boxm2_volm_desc_ex_matcher(depth_map_scene_sptr const& dms,
                             vcl_vector<double> const& radius,
                             unsigned const& norients = 3,
                             unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                             unsigned char const& initial_mag = 0)
  : dms_(dms), radius_(radius), norients_(norients), nlands_(nlands), initial_mag_(initial_mag)
  { }

  //: Destructor
  ~boxm2_volm_desc_ex_matcher() {}

  //: check given threshold is valid or not for generate scaled probability map
  virtual bool check_threshold(volm_desc_sptr const& query, float& thres_value);

  //: Compare two descriptor a and b using the similarity method implemented in descriptor a
  virtual float score(volm_desc_sptr const& query, volm_desc_sptr const& index);

  //: Create a volumetric existance descriptor for the query image
  virtual volm_desc_sptr create_query_desc();

  virtual vcl_string get_index_type_str() { return volm_desc_ex_indexer::name_; }


private:
  //: query depth_map_scene
  depth_map_scene_sptr  dms_;
  //: parameters related to volumetric existance descriptor
  vcl_vector<double> radius_;
  unsigned         norients_;
  unsigned           nlands_;
  unsigned char initial_mag_;

};

#endif // boxm2_volm_desc_ex_matcher_h_