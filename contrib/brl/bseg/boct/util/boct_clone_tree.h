// This is brl/bseg/boct/util/boct_clone_tree.h
#ifndef boct_clone_tree_h_
#define boct_clone_tree_h_
//:
// \file
// \brief  Set of functions to clone an octree
//
// \author Isabel Restrepo
// \date   Sept 16, 2011
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boct/boct_tree.h>

#include "boct_construct_tree.h"

//: Clone only from leaves at given level
template<class T_loc, class T_data, class T_data_to>
boct_tree<T_loc,T_data_to>* boct_clone_to_type(boct_tree<T_loc, T_data>* tree, short level, T_data_to default_val,
                                               boct_tree<T_loc, bool>* valid_tree= NULL)
{
  std::vector<boct_tree_cell<T_loc, T_data>*> cells = tree->leaf_cells_at_level(level);
  std::vector<boct_tree_cell<T_loc, T_data_to> > cloned_cells;

  if (!valid_tree) {
    for (unsigned i=0; i<cells.size(); i++) {
      cloned_cells.push_back(boct_tree_cell<T_loc, T_data_to>(cells[i]->code_));
    }
  }
  else {
    std::vector<boct_tree_cell<T_loc, bool>*> valid_cells = valid_tree->leaf_cells_at_level(level);
    for (unsigned i=0; i<cells.size(); i++) {
      if (valid_cells[i]->data())
        cloned_cells.push_back(boct_tree_cell<T_loc, T_data_to>(cells[i]->code_));
    }
  }

  boct_tree_cell<T_loc, T_data_to>* cloned_root = boct_construct_tree(cloned_cells, tree->number_levels(), default_val );
  boct_tree<T_loc,T_data_to>* cloned_tree = new boct_tree<T_loc,T_data_to>(cloned_root,  tree->number_levels());
  cloned_tree->set_bbox(tree->bounding_box());
  return cloned_tree;
}

#endif
