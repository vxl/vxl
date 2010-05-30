//:
// \file
// This file contains complete specializations of the functions

#include "boct_tree_cell.h"

//: Averages the value of the 8 children in a dynamic programming manner
// Specialization for float. Partial specialization is not part of c++ standard for this case
// so we have to explicitly write all template parameters
template<>
void boct_tree_cell<short,float>::set_data_to_avg_children()
{
  //If cell is a leaf, it should have its data already stored
  if (this-> is_leaf())
    return;

  //If cell isn't a leaf fill it with the average of their children
  data_=0; //remove previous data
  for (unsigned i=0; i<8; i++) {
    children_[i].set_data_to_avg_children();
    data_ += children_[i].data()/8.0f;
  }

  return;
}
