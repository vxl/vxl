#ifndef boxm_scene_base_h_
#define boxm_scene_base_h_
//:
// \file
// \brief  The main class to keep the 3D world data and pieces
//
// \author Gamze Tunali
// \date Apr 14, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_smart_ptr.h>

#include "boxm_sample.h"

class boxm_scene_base : public vbl_ref_count
{
public:
  boxm_scene_base() 
    : app_model_(BOXM_APM_UNKNOWN), tree_level_set_(false), scene_path_(""), block_pref_(""),multi_bin_(false) {}

  virtual ~boxm_scene_base(){}

  boxm_apm_type appearence_model() { return app_model_; }

  void set_appearence_model(boxm_apm_type model) { app_model_ = model; }

  void set_octree_levels(unsigned max, unsigned init) 
   { max_tree_level_=max; init_tree_level_=init; tree_level_set_=true; }

  void set_paths(vcl_string scene_path, vcl_string block_prefix)
   { scene_path_ = scene_path;  block_pref_=block_prefix; }

  bool multi_bin(){return multi_bin_;}
 protected:
  boxm_apm_type app_model_;

  bool multi_bin_;

  //: maximum octree levels allowed
  unsigned max_tree_level_;

  //: number of initial octree levels created at the tree creation step
  unsigned init_tree_level_;

  //: it is set to treu when max and init tree levels are given
  bool tree_level_set_;

  vcl_string scene_path_;

  vcl_string block_pref_;
};

typedef vbl_smart_ptr<boxm_scene_base> boxm_scene_base_sptr;


#endif
