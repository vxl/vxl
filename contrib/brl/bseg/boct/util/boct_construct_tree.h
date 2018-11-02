// This is brl/bseg/boct/util/boct_construct_tree.h
#ifndef boct_construct_tree_h
#define boct_construct_tree_h
//:
// \file
// \brief A Utility function to construct a boct_tree
// \author Isabel Restrepo mir@lems.brown.edu
// \date  16-Sep-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <boct/boct_tree.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Construct a tree from leaf nodes. Cells are initialized to the default value given
template <class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>* boct_construct_tree(std::vector<boct_tree_cell<T_loc, T_data> >& leaf_nodes,
                                                  short num_levels, T_data default_val)
{
  // create an empty tree
  boct_loc_code<T_loc> code;
  boct_tree_cell<T_loc,T_data>* root;
  if (num_levels>0) {
    code.set_code(0,0,0);
    code.set_level(num_levels-1);
    root=new boct_tree_cell<T_loc,T_data>(code);
    root->set_data(default_val);
  }
  else {
    std::cerr << "boct_tree: the tree max level is 0, cannot create a tree!\n";
    return nullptr;
  }

  for (unsigned i=0; i<leaf_nodes.size(); i++)
  {
    boct_tree_cell<T_loc, T_data>& cell = leaf_nodes[i];
    boct_loc_code<T_loc> loccode=cell.code_;
    int level=loccode.level;

    // temporary pointer to traverse
    boct_tree_cell<T_loc,T_data>* curr_cell=root;
    short curr_level=num_levels-1;

    while (curr_level>level)
    {
      if (curr_cell->is_leaf()) {
        curr_cell->split(default_val);
      }
      short child_index=loccode.child_index(curr_level);
      if (child_index < 0)
        std::cout << "ERROR 1: child_index is " << child_index << std::endl;
      curr_cell=curr_cell->children()+child_index;
      --curr_level;
    }

    if (curr_cell->code_.isequal(&loccode))
      // the place of the cell is found, put the data in
      curr_cell->set_data(cell.data());
    else
      std::cerr << "WRONG ERROR CODE OR CELL FOUND!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
  }
  return root;
}

#endif
