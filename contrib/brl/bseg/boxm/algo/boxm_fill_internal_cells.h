// This is brl/bseg/boxm/algo/boxm_fill_internal_cells.h
#ifndef boxm_fill_internal_cells_h
#define boxm_fill_internal_cells_h
//:
// \file
// \brief Class in charged of filling the intermediate nodes of a tree.
// \author Maria Isabel Restrepo mir@lems.brown.edu
// \date  Feb 3, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A class to combine children cells and fill intermediate nodes
template <class T_data>
class boxm_fill_internal_cells
{
 public:
  typedef boct_tree<short,T_data> tree_type;

  boxm_scene<boct_tree<short,T_data> > * traverse_and_fill(boxm_scene<boct_tree<short,T_data> > *scene_in)
  {
    // Most parameters of the output scene are the same as those of the input scene
    // but we need to change the mode of the scene to be saving internal nodes
    bool save_internal_nodes = true;
    boxm_scene<tree_type> *scene_out = new boxm_scene<tree_type>(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(),
                                                                 scene_in->world_dim(), scene_in->max_level(), scene_in->init_level(),
                                                                 false, save_internal_nodes);
    std::string block_pref = scene_in->block_prefix() + "_all_nodes";
    scene_out->set_paths(scene_in->path(), block_pref);
    scene_out->set_appearance_model(scene_in->appearence_model());

    scene_out->write_scene(block_pref + ".xml");

    //Iterate through the blocks
    boxm_block_iterator<tree_type> iter_in = scene_in->iterator();
    boxm_block_iterator<tree_type> iter_out = scene_out->iterator();

    iter_in.begin();
    iter_out.begin();

    while (!iter_in.end() && !iter_out.end())
    {
      scene_in->load_block(iter_in.index());
      scene_out->load_block(iter_out.index());

      tree_type *tree_in= (*iter_in)->get_tree();
      // Copy input tree into output tree and fill
      tree_type *tree_out = tree_in->clone();
      this->traverse_and_fill(tree_out);
      (*iter_out)->init_tree(tree_out);
      std::cout << "writing block: " << iter_out.index() <<std::endl;
      scene_out->write_active_block();

      iter_in++; iter_out++;
    }

    return scene_out;
  }

 protected:

  //: Traverses a tree and fills intermediate cells with the average of the 8 children.
  // If there is need to fill cells with something different to the average, this function can be made abstract and
  // write specializations. As in the template method design pattern
  void traverse_and_fill(tree_type *tree_to_fill)
  {
    tree_to_fill->fill_with_average();
  }
};


#endif
