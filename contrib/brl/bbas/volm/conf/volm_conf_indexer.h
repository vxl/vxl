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
//   Yi Dong     SEP--2014    added height attribute
// \endverbatim
//

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  volm_conf_indexer() : out_index_folder_(""), loc_root_(nullptr), current_leaf_id_(0), tile_id_(0) { loc_leaves_.clear(); }
  volm_conf_indexer(std::string  out_index_folder)
    : out_index_folder_(std::move(out_index_folder)), loc_root_(nullptr), current_leaf_id_(0), tile_id_(0) { loc_leaves_.clear(); }

  ~volm_conf_indexer() override = default;

  // =================== access =====================
  volm_geo_index_node_sptr loc_root()  const { return loc_root_; }
  std::string out_folder()              const { return out_index_folder_; }
  unsigned tile_id()                   const { return tile_id_; }
  unsigned current_leaf_id()           const { return current_leaf_id_; }
  std::vector<volm_geo_index_node_sptr>& loc_leaves() { return loc_leaves_; }

  // ================== methods =====================

  //: load location database for given tile
  bool load_loc_hypos(std::string const& geo_hypo_folder, unsigned const& tile_id);

  //: return the name of the indexer
  virtual std::string get_index_name() const = 0;

  //: generate parameter files for different indexer
  virtual bool write_params_file();

  //: handles any necessary loading during indexing as current indexer switches processing from one location leaf to next leaf
  virtual bool get_next() { return true; }

  //: iterate over each locations in each leaf and run 'extract' to construct index at each location
  bool index(float const& buffer_capacity, int const& min_leaf_id, int const& max_leaf_id);

  //: function to construct index for a location
  virtual bool extract(double const& lon, double const& lat, double const& elev, std::vector<volm_conf_object>& values) = 0;

protected:
  //: output folder where the created indices will be stored
  std::string out_index_folder_;
  //: output file pre
  std::stringstream out_file_name_pre_;
  //: 2d geo_index for location hypotheses
  volm_geo_index_node_sptr loc_root_;
  std::vector<volm_geo_index_node_sptr> loc_leaves_;
  unsigned current_leaf_id_;
  unsigned tile_id_;
};

#endif // volm_conf_indexer_h_
