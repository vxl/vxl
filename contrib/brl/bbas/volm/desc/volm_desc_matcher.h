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

#include <volm/desc/volm_desc.h>
#include <volm/desc/volm_desc_indexer.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_string.h>
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
  volm_desc_matcher() {}

  //: Destructor
  virtual ~volm_desc_matcher() {}

  //: Comparison method to calculate the similarity of descriptor a and b, return a score from 0 to 1
  virtual float score(volm_desc_sptr const& query, volm_desc_sptr const& index) {return 0;}

  //: Create volumetric descriptor for the query image
  virtual volm_desc_sptr create_query_desc() = 0;

  //: check given threshold is valid or not for generate scaled probability map
  virtual bool check_threshold(volm_desc_sptr const& query, float& thres_value) { return true; }

  //: Execute match algorithm implemented
  bool matcher(volm_desc_sptr const& query,
               vcl_string const& geo_hypo_folder,
               vcl_string const& desc_index_folder,
               float buffer_capacity,
               unsigned const& tile_id);

  //: write the matcher scores
  bool write_out(vcl_string const& out_folder, unsigned const& tile_id);

  //: generate probability map
  bool create_prob_map(vcl_string const& geo_hypo_folder,
                       vcl_string const& out_folder,
                       unsigned const& tile_id,
                       volm_tile tile,
                       vgl_point_3d<double> const& gt_loc,
                       float& gt_score);

  bool create_empty_prob_map(vcl_string const& out_folder, unsigned tile_id, volm_tile& tile);

  //: need a specialized prob map generator for a random matcher, cause random matcher do not need to create a desc_index nor save binary score files
  bool create_random_prob_map(vnl_random& rng, vcl_string const& geo_hypo_folder, vcl_string const& out_folder, unsigned tile_id, volm_tile& tile);

  //: generate scaled probability map
  static bool create_scaled_prob_map(vcl_string const& out_folder,
                                     volm_tile tile,
                                     unsigned const& tile_id,
                                     float const& ku,
                                     float const& kl,
                                     float const& thres_value);

  //: generate candidate list
  static bool create_candidate_list(vcl_string const& map_root,
                                    vcl_string const& cand_root,
                                    unsigned const& threshold,
                                    unsigned const& top_size,
                                    float const& ku,
                                    float const& kl,
                                    float const& thres_value,
                                    vcl_string const& query_name,
                                    vcl_string const& world_id);

  virtual vcl_string get_index_type_str() = 0;

protected:
  // output scores (score per location, the vector contains scores for locations in a tile)
  vcl_vector<volm_score_sptr> score_all_;

};

#endif  // volm_desc_matcher_h_
