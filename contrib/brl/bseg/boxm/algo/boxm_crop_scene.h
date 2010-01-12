// This is brl/bseg/boxm/algo/boxm_crop_scene_h
#ifndef boxm_crop_scene_h
#define boxm_crop_scene_h

//:
// \file
// \brief  Template functions to create a new scene that is a cropped portion of another
// \author Isabel Restrepo mir@lems.brown.edu
// \date  1/11/10
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vgl/vgl_box_3d.h>
#include <boxm/boxm_scene.h>


template <class T_data> 
 boxm_scene<boct_tree<short, T_data> > *  boxm_crop_scene(boxm_scene<boct_tree<short,T_data> > *scene_in,vgl_box_3d<double> bbox)
{
  //only one-block scenes are supported for now
  if(!(scene_in->world_dim().x() == 1 && scene_in->world_dim().y()==1 && scene_in->world_dim().z()==1))
  {
    vcl_cerr << "In bvxm_crop_scene -- Input scene has more than one block, which is not supported" << vcl_endl;
    return 0;
  }
  
  //parameters of the output scene are the same as thos of the input scene  
  boxm_scene<boct_tree<short, T_data> > *scene_out =
  new boxm_scene<boct_tree<short, T_data> >(scene_in->lvcs(), bbox.min_point(),vgl_vector_3d<double>(bbox.width(), bbox.height(), bbox.depth()),scene_in->world_dim());
  
  scene_out->set_paths(scene_in->path(), "cropped");
  scene_out->set_appearance_model(scene_in->appearence_model());
  scene_out->clean_scene();

  //get the tree inside the bloack and locate region
  boxm_block_iterator<boct_tree<short, T_data> > iter_in = scene_in->iterator();
  boxm_block_iterator<boct_tree<short, T_data> > iter_out = scene_out->iterator();
  iter_in.begin();
  iter_out.begin();
  
  //there is no need to increase iterator, since there should only be one block in the scene
  scene_in->load_block(iter_in.index());
  scene_out->load_block(iter_out.index());
  boct_tree<short, T_data>  *tree_in= (*iter_in)->get_tree();
  
  boct_tree_cell<short,T_data>* root = tree_in->locate_region_global(bbox);
  
  //output tree is created from leaves and root level (This may need to change to that non-leaf cells get copied as well)
  boct_tree<short, T_data >  *tree_out = tree_in->clone_subtree(root, tree_in->root_level());
  
  //write the output tree
  (*iter_out)->init_tree(tree_out);
  scene_out->write_active_block();
  scene_out->write_scene("/cropped_scene.xml");
  
  return scene_out;
  
}                 
                     

#endif
