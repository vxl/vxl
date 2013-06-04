// This is brl/bbas/volm/desc/volm_desc_ex_matcher.h
#ifndef volm_desc_ex_matcher_h_
#define volm_desc_ex_matcher_h_
//:
// \file
// \brief  A class to match query volumetric existance descriptor to existance indices
//
// \author Yi Dong
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include "volm_desc_matcher.h"
#include <volm/desc/volm_desc_ex.h>
#include <volm/desc/volm_desc_indexer.h>
#include <vcl_vector.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>

class volm_desc_ex_matcher : public volm_desc_matcher
{
public:
  //: Defaule constructor
  volm_desc_ex_matcher() {}

  //: Constructor
  volm_desc_ex_matcher(depth_map_scene_sptr const& dms,
                       vcl_vector<double> const& radius,
                       vcl_string const& name, 
                       unsigned const& norients = 3,
                       unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                       unsigned char const& initial_mag = 0)
  : dms_(dms), radius_(radius), norients_(norients), nlands_(nlands), initial_mag_(initial_mag)
  { name_ = name;}

  //: Compare two descriptor a and b using the similarity method implemented in descriptor a
  float score(volm_desc_sptr const& a, volm_desc_sptr const& b);

  //: Create a volumetric existance descriptor for the query image
  volm_desc_sptr create_query_desc();

  //: ececute the matcher 
  bool matcher(volm_desc_sptr const& query,
               vcl_string const& geo_hypo_folder,
               vcl_string const& desc_index_folder,
               unsigned const& tile_id);

  //: write out the result
  bool write_out(vcl_string const& out_folder, unsigned const& tile_id);

  //: Destructor
  ~volm_desc_ex_matcher() {}

private:
  //: query depth_map_scene
  depth_map_scene_sptr  dms_;
  
  //: parameters related to volumetric existance descriptor
  vcl_vector<double> radius_;
  unsigned         norients_;
  unsigned           nlands_;
  unsigned char initial_mag_;

};

#endif  //_VOLM_DESC_EX_MATCHER_H
