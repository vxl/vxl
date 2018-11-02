// This is brl/bseg/boxm/algo/boxm_linear_operations.h
#ifndef boxm_linear_operations_h
#define boxm_linear_operations_h
//:
// \file
// \brief Functions to perfoms linear operations on boxm scenes and/or blocks
// \author Isabel Restrepo mir@lems.brown.edu
// \date  2-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <boxm/boxm_block.h>
#include <boct/boct_tree.h>
#include <cassert>
#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

//: Computes the linear combination of two blocks in place. i.e block1 = s1*block1 +  s2*block2
template<class T_data>
bool boxm_linear_combination(boxm_block<boct_tree<short, T_data> > *block1, boxm_block<boct_tree<short, T_data> > *block2, float s1, float s2)
{
  boct_tree<short, T_data> *tree1 = block1->get_tree();
  boct_tree<short, T_data> *tree2 = block2->get_tree();

  std::vector<boct_tree_cell<short, T_data>* > cells1 = tree1->leaf_cells();
  std::vector<boct_tree_cell<short, T_data>* > cells2 = tree2->leaf_cells();

  //iterators
  typename std::vector<boct_tree_cell<short, T_data>* >::iterator it1 = cells1.begin();
  typename std::vector<boct_tree_cell<short, T_data>* >::iterator it2 = cells2.begin();

  for (; (it1!=cells1.end())&&(it2!= cells2.end()); it1++, it2++)
  {
    boct_tree_cell<short,T_data> *cell1 = *it1;
    boct_tree_cell<short,T_data> *cell2 = *it2;

#ifdef DEBUG
    //if level and location code of cells isn't the same quit
    boct_loc_code<short> code1 = cell1->get_code();
    boct_loc_code<short> code2 = cell2->get_code();
    if (!(code1.isequal(code2)))
    {
      std::cerr << "In boxm_linear_combination: Blocks must have the same structure\n";
      return false;
    }
#endif

    cell1->set_data(s1*cell1->data() + s2*cell2->data());
  }
  return true;
}

#endif
