#pragma once
// This is brl/bbas/volm/desc/volm_desc_matcher.h
#ifndef volm_desc_matcher_h_
#define volm_desc_matcher_h_
//:
// \file
// \brief  A base class for matchers using volumetric descriptors
//
// \author Yi Dong
// \date May 29, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <iostream>
#include <string>
#include <volm/desc/volm_desc.h>
#include <volm/desc/volm_desc_indexer.h>
#include <vbl/vbl_smart_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_buffered_index.h>
#include <vnl/vnl_random.h>

class volm_desc_matcher;
typedef vbl_smart_ptr<volm_desc_matcher> volm_desc_matcher_sptr;

class volm_desc_matcher : public vbl_ref_count
{
public:
  //: Default constructor
  volm_desc_matcher() = default;

  //: Destructor
  ~volm_desc_matcher() override = default;

  //: Comparison method to calculate the similarity of descriptor a and b, return a score from 0 to 1
  virtual float score(volm_desc_sptr const& query, volm_desc_sptr const& index) {return 0;}

  //: Create volumetric descriptor for the query image
  virtual volm_desc_sptr create_query_desc() = 0;

  //: check given threshold is valid or not for generate scaled probability map
  virtual bool check_threshold(volm_desc_sptr const& query, float& thres_value) { return true; }

  //: Execute match algorithm implemented
  bool matcher(volm_desc_sptr const& query,
               std::string const& geo_hypo_folder,
               std::string const& desc_index_folder,
               float buffer_capacity,
               unsigned const& tile_id);

  //: write the matcher scores
  bool write_out(std::string const& out_folder, unsigned const& tile_id);

  //: generate probability map
  bool create_prob_map(std::string const& geo_hypo_folder,
                       std::string const& out_folder,
                       unsigned const& tile_id,
                       volm_tile tile,
                       vgl_point_3d<double> const& gt_loc,
                       float& gt_score);

  bool create_empty_prob_map(std::string const& out_folder, unsigned tile_id, volm_tile& tile);

  //: need a specialized prob map generator for a random matcher, cause random matcher do not need to create a desc_index nor save binary score files
  bool create_random_prob_map(vnl_random& rng, std::string const& geo_hypo_folder, std::string const& out_folder, unsigned tile_id, volm_tile& tile);

  //: generate scaled probability map
  static bool create_scaled_prob_map(std::string const& out_folder,
                                     volm_tile tile,
                                     unsigned const& tile_id,
                                     float const& ku,
                                     float const& kl,
                                     float const& thres_value);

  //: generate candidate list
  static bool create_candidate_list(std::string const& map_root,
                                    std::string const& cand_root,
                                    unsigned const& threshold,
                                    unsigned const& top_size,
                                    float const& ku,
                                    float const& kl,
                                    float const& thres_value,
                                    std::string const& query_name,
                                    std::string const& world_id);

  virtual std::string get_index_type_str() = 0;

protected:
  // output scores (score per location, the vector contains scores for locations in a tile)
  std::vector<volm_score_sptr> score_all_;

};

#endif  // volm_desc_matcher_h_
