//This is brl/bseg/boxm2/volm/conf/boxm2_volm_conf_matcher.h
#ifndef boxm2_volm_conf_matcher_h_
#define boxm2_volm_conf_matcher_h_
//:
// \file
// \brief  A class to match a configurational query to pre-created configurational indices.  The output will be
//         a measurement (from 0 to 1) to quantify the similarity of configuration in the query and the index
//         Note that the matcher should be able to perform on both 2-d indices and 3-d indices
//
// \author Yi Dong
// \date August 27, 2014
// \verbatim
//   Modifications
//    <none yet>
// \endverbatim
//

#include <vgl/vgl_polygon.h>
#include <volm/volm_loc_hyp.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/conf/volm_conf_object.h>
#include <volm/conf/volm_conf_query.h>
#include <volm/conf/volm_conf_2d_indexer.h>
#include <volm/conf/volm_conf_score.h>

class boxm2_volm_conf_matcher
{
public:
  // =============== constructor ======================

  //: default constructor
  boxm2_volm_conf_matcher() = default;
  boxm2_volm_conf_matcher(volm_conf_query_sptr const& query,
                          unsigned const& tile_id,
                          std::vector<volm_geo_index_node_sptr>  loc_leaves,
                          std::string  index_folder,
                          std::string  out_folder,
                          std::string  cand_folder,
                          float const& buffer_capacity = 2.0);
  //: constructor to create query online
  boxm2_volm_conf_matcher(volm_camera_space_sptr const& cam_space, depth_map_scene_sptr const& depth_scene,
                          unsigned const& tile_id,
                          std::vector<volm_geo_index_node_sptr>  loc_leaves,
                          std::string  index_folder,
                          std::string  out_folder,
                          std::string  cand_folder,
                          float const& buffer_capacity = 2.0,
                          unsigned tol_in_pixel = 25);

  ~boxm2_volm_conf_matcher() = default;

  // =================== access =======================
  volm_conf_query_sptr query()    const { return query_; }
  std::string index_folder()       const { return index_folder_; }
  float buffer_capacity()         const { return buffer_capacity_; }

  std::vector<volm_geo_index_node_sptr>& loc_leaves() { return loc_leaves_; }

  // =================== method =======================

  //: configurational matcher on location leaf with leaf_id (if leaf_id is -1, matcher will operate on all leaves)
  int conf_match_cpp(std::string const& index_name, bool const& use_height = true);
  int conf_match_cpp_no_candidate(std::string& index_name, bool const& use_height = true);

private:
  //: configurational query
  volm_conf_query_sptr query_;

  //: tile id where matcher operates on
  unsigned tile_id_;

  //: configurational index folder
  std::string index_folder_;

  //: location database
  std::vector<volm_geo_index_node_sptr> loc_leaves_;

  //: matcher related parameters
  float buffer_capacity_;  // in GB

  //: candidate polygon
  std::string cand_folder_;

  //: output folder
  std::string out_folder_;

  //: matching query with a single location index
  bool matching(std::vector<volm_conf_object> const& values, volm_conf_score& score, bool const& use_height = false);

#if 0
  //: matching given a reference object point in index
  void match_to_reference(volm_conf_object const& ref_i, volm_conf_object_sptr const& ref_q,
                           std::vector<volm_conf_object_sptr> const& obj_q,
                           std::map<unsigned char, std::vector<volm_conf_object> >& obj_i,
                           float& score,
                           std::vector<volm_conf_object>& matched_objs);
#endif

  void match_to_reference_h(volm_conf_object const& ref_i, volm_conf_object_sptr const& ref_q,
                            std::vector<volm_conf_object_sptr> const& obj_q,
                            std::map<unsigned char, std::vector<volm_conf_object> >& obj_i,
                            float& score,
                            std::vector<volm_conf_object>& matched_objs,
                            bool const& use_height = true);

};

#endif // boxm2_volm_conf_matcher_h_
