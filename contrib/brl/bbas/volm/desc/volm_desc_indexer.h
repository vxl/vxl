#pragma once
// This is brl/bbas/volm/desc/volm_desc_indexer.h
#ifndef volm_desc_indexer_h_
#define volm_desc_indexer_h_
//:
// \file
// \brief  A class to extract descriptors from an ROI using its geoindex
//
// \author Ozge C. Ozcanli
// \date May 29, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>


class volm_desc_indexer : public vbl_ref_count
{
public:
  // constructor
  volm_desc_indexer(vcl_string const& out_index_folder) : out_index_folder_(out_index_folder) {}

  //: load the volm_geo_index for given tile
  bool load_tile_hypos(vcl_string const& geo_hypo_folder, int tile_id);

  //: generate parameter files for different indexer
  virtual bool write_params_file();

  // handles any necessary loading during indexing when it switches processing from one leaf to the next leaf
  virtual bool get_next() { return true; }

  //: go over each hypo in each leaf and run 'extract' at each location
  bool index(float buffer_capacity, int min_leaf_id, int max_leaf_id);

  virtual bool extract(double lat, double lon, double elev, vcl_vector<unsigned char>& values) = 0;

  virtual vcl_string get_index_type_str() = 0;
  
  //: each driving indexer should overwrite with the size of the descriptor
  virtual unsigned layer_size() = 0;


public:
  vcl_string out_index_folder_;
  vcl_stringstream out_file_name_pre_;
  volm_geo_index_node_sptr root_;
  vcl_vector<volm_geo_index_node_sptr> leaves_;
  unsigned current_leaf_id_;
  unsigned tile_id_;
};

#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<volm_desc_indexer> volm_desc_indexer_sptr;

#endif  //_VOLM_DESC_INDEXER_H
