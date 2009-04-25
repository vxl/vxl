#ifndef boxm_refine_h_
#define boxm_refine_h_
//:
// \file
#include <boct/boct_tree.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_scene.h>


//: This method refines the blocks (octrees) of the scene
// If there are some cells with big values they are spitted into new child cells;
// the data of the cell is copied to the children's data. The purpose of this
// method is to elaborate the octree at the areas where more details exist.

template <class T_loc, boxm_apm_type APM>
void boxm_refine(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                 float threshold)
{
  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;

  boxm_block_iterator<tree_type> iter(&scene);

  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    tree_type* tree = block->get_tree();
    // get the leaf nodes of the octree
    vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> >*> leaf_nodes = tree->leaf_cells();

    for (unsigned i=0; i<leaf_nodes.size(); i++) {
      boct_tree_cell<T_loc, boxm_sample<APM> >* cell = leaf_nodes[i];

      // check the data value
      boxm_sample<APM> data = cell->data();
      if (data.alpha > threshold) {
        cell->split();
      }
    }
  }
}

#endif
