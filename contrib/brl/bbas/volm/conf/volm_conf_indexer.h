//This is brl/bbas/volm/conf/volm_conf_indexer.h
#ifndef volm_conf_indexer_h_
#define volm_conf_indexer_h_
//:
// \file
// \brief  A class to construct 2d configuration index for given locations
//
// \author Yi Dong
// \date August 25, 2014
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/conf/volm_conf_object.h>
#include <volm/conf/volm_conf_buffer.h>


class volm_conf_indexer;
typedef vbl_smart_ptr<volm_conf_indexer> volm_conf_indexer_sptr;

class volm_conf_indexer : public vbl_ref_count
{
public:
  // ================ constructor ===================
  volm_conf_indexer() : out_index_folder_(""), loc_root_(0), current_leaf_id_(0), tile_id_(0) { loc_leaves_.clear(); }
  volm_conf_indexer(vcl_string const& out_index_folder)
    : out_index_folder_(out_index_folder), loc_root_(0), current_leaf_id_(0), tile_id_(0) { loc_leaves_.clear(); }

  ~volm_conf_indexer() {}

  // =================== access =====================
  volm_geo_index_node_sptr loc_root()  const { return loc_root_; }
  vcl_string out_folder()              const { return out_index_folder_; }
  unsigned tile_id()                   const { return tile_id_; }
  unsigned current_leaf_id()           const { return current_leaf_id_; }
  vcl_vector<volm_geo_index_node_sptr>& loc_leaves() { return loc_leaves_; }

  // ================== methods =====================

  //: load location database for given tile
  bool load_loc_hypos(vcl_string const& geo_hypo_folder, unsigned const& tile_id);

  //: return the name of the indexer
  virtual vcl_string get_index_name() const = 0;

  //: generate parameter files for different indexer
  virtual bool write_params_file();

  //: handles any necessary loading during indexing as current indexer switches processing from one location leaf to next leaf
  virtual bool get_next() { return true; }

  //: iterate over each locations in each leaf and run 'extract' to construct index at each location
  bool index(float const& buffer_capacity, int const& min_leaf_id, int const& max_leaf_id);

  //: function to construct index for a location
  virtual bool extract(double const& lon, double const& lat, double const& elev, vcl_vector<volm_conf_object>& values) = 0;

protected:
  //: output folder where the created indices will be stored
  vcl_string out_index_folder_;
  //: output file pre
  vcl_stringstream out_file_name_pre_;
  //: 2d geo_index for location hypotheses
  volm_geo_index_node_sptr loc_root_;
  vcl_vector<volm_geo_index_node_sptr> loc_leaves_;
  unsigned current_leaf_id_;
  unsigned tile_id_;
};

#endif // volm_conf_indexer_h_