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

  //: generate scaled probability map
  static bool create_scaled_prob_map(vcl_string const& out_folder,
                                     volm_tile tile,
                                     unsigned const& tile_id,
                                     float const& ku,
                                     float const& kl,
                                     unsigned const& num_valid_bins,
                                     float const& thres_ratio);

  //: generate candidate list
  static bool create_candidate_list(vcl_string const& map_root,
                                    vcl_string const& cand_root,
                                    vcl_string const& geo_hypo_folder_a,
                                    vcl_string const& geo_hypo_folder_b,
                                    unsigned const& threshold,
                                    unsigned const& top_size,
                                    float const& ku,
                                    float const& kl,
                                    unsigned const& num_valid_bins,
                                    float const& thres_value,
                                    unsigned const& test_id,
                                    unsigned const& img_id);

  virtual vcl_string get_index_type_str() = 0;

protected:
  // output scores (score per location, the vector contains scores for locations in a tile)
  vcl_vector<volm_score_sptr> score_all_;

};

#endif  // volm_desc_matcher_h_
