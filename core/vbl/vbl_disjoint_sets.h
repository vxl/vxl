// This is core/vbl/vbl_disjoint_sets.h
#ifndef vbl_disjoint_sets_h_
#define vbl_disjoint_sets_h_
//:
// \file
// \brief implements a disjoint set (union, find)
// \author Emil Stefanov
//
// \verbatim
// Adapted to VXL by J. Mundy Feb. 14, 2013
// \endverbatim
//-----------------------------------------------------------------------------

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vbl_array_1d.h"
// Disjoint Set Data Structure
// Author: Emil Stefanov
// Date: 03/28/06
// Implementation is as described in http://en.wikipedia.org/wiki/Disjoint-set_data_structure
// Copyrighted according to the MIT license
// http://opensource.org/licenses/mit-license.html
// 12/17/2016 - JLM
// changed node storage to vbl_array_1d so that delete is fast
class vbl_disjoint_sets
{
 public:

  // Create an empty vbl_disjoint_sets data structure
  vbl_disjoint_sets();
  //: Create a vbl_disjoint_sets data structure with a specified number of elements (with element id's from 0 to count-1)
  vbl_disjoint_sets(int count);
  //:Copy constructor
  vbl_disjoint_sets(const vbl_disjoint_sets & s);
  //: Destructor
  ~vbl_disjoint_sets();

  //: Find the set identifier that an element currently belongs to.
  int find_set(int element) const;

  //: Combine two sets into one.
  // All elements in those two sets will share the same set id that
  // can be retrieved using find_set.
  void set_union(int set_id1, int set_id2);

  // Add a specified number of elements to the data structure.
  // The element id's of the new elements are numbered
  // consequitively starting with the first never-before-used element_id.
  void add_elements(int num_to_add);

  // Returns the number of elements currently in the data structure.
  int num_elements() const;

  //: Returns the number of elements in set specified by set_id
  int size(int set_id) const;

  //: Returns the number of sets
  int num_sets() const;

 private:

  // Internal node data structure used for representing an element
  struct node
  {
  node():rank(0), index(0), parent(nullptr), size(1){}
    //: represents the approximate max height of the node in its subtree
    int rank;
    int index; // The index of the element the node represents
    node* parent; // The parent node of the node
    int size; // the number of elements in the set
  };
  int num_elements_; // the number of elements
  int num_sets_; // the number of sets
  //  std::vector<node*> nodes_; // the list of nodes representing the elements
  vbl_array_1d<node> nodes_; // changed to vbl_array since deleting was very slow
};

#endif // vbl_disjoint_sets_h_
