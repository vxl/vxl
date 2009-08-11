#ifndef boxm_aux_scene_h_
#define boxm_aux_scene_h_
//:
// \file
// \brief  an auxiliary scene keeps a set of octrees for a given main scene
//
// \author Gamze Tunali
// \date   July 31, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vgl/vgl_box_3d.h>

#include <boct/boct_tree.h>
#include <boct/boct_tree_cell_reader.h>
#include <boxm/boxm_scene.h>
#include "boxm_opt_sample.h"

template <class T_loc, class T, class T_AUX>
class boxm_aux_scene
{
 public:
   //typedef boct_tree<T_loc, boxm_sample<T> > scene_type;
  //: when lvcs is not avialable
   typedef  boct_tree<T_loc, T> tree_type;
   typedef  boct_tree<T_loc, T_AUX> aux_tree_type;
   boxm_aux_scene(boxm_scene<tree_type>* scene, vcl_string suffix);

   ~boxm_aux_scene(){delete aux_scene_;}

   void init_scene(){}

 // boxm_block<aux_tree_type> get_block(vgl_point_3d<int> block_idx) {return aux_scene_->load_block(block_idx); }

  //: get an incremental reader for a scene block
  boct_tree_cell_reader<T_loc, T_AUX >* get_block_incremental(vgl_point_3d<int> block_idx);

 private:
  boxm_scene<aux_tree_type>* aux_scene_;
  vcl_string aux_storage_dir_;
};

#endif
