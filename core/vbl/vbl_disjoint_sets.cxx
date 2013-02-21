// Disjoint Set Data Structure
// Author: Emil Stefanov
// Date: 03/28/06
// Implementation is as described in http://en.wikipedia.org/wiki/Disjoint-set_data_structure

#include <vcl_cassert.h>
#include <vcl_cstddef.h>

#include "vbl_disjoint_sets.h"


vbl_disjoint_sets::vbl_disjoint_sets()
{
  num_elements_ = 0;
  num_sets_ = 0;
}

vbl_disjoint_sets::vbl_disjoint_sets(int count)
{
  num_elements_ = 0;
  num_sets_ = 0;
  add_elements(count);
}

vbl_disjoint_sets::vbl_disjoint_sets(const vbl_disjoint_sets & s)
{
  this->num_elements_ = s.num_elements_;
  this->num_sets_ = s.num_sets_;

  // Copy nodes
  nodes_.resize(num_elements_);
  for (int i = 0; i < num_elements_; ++i)
    nodes_[i] = new node(*s.nodes_[i]);

  // Update parent pointers to point to newly created nodes rather than the old ones
  for (int i = 0; i < num_elements_; ++i)
    if (nodes_[i]->parent != NULL)
      nodes_[i]->parent = nodes_[s.nodes_[i]->parent->index];
}

vbl_disjoint_sets::~vbl_disjoint_sets()
{
  for (int i = 0; i < num_elements_; ++i)
    delete nodes_[i];
  nodes_.clear();
  num_elements_ = 0;
  num_sets_ = 0;
}

// Note: some internal data is modified for optimization even though this method is consant.
int vbl_disjoint_sets::find_set(int element_id) const
{
  assert(element_id < num_elements_);

  node* curnode;

  // Find the root element that represents the set which `element_id` belongs to
  curnode = nodes_[element_id];
  while (curnode->parent != 0)
    curnode = curnode->parent;
  node* root = curnode;

  // Walk to the root, updating the parents of `element_id`. Make those elements the direct
  // children of `root`. This optimizes the tree for future find_set invokations.
  curnode = nodes_[element_id];
  while (curnode != root)
  {
    node* next = curnode->parent;
    curnode->parent = root;
    curnode = next;
  }

  return root->index;
}

void vbl_disjoint_sets::set_union(int setId1, int setId2)
{
  assert(setId1 < num_elements_);
  assert(setId2 < num_elements_);

  if (setId1 == setId2)
    return; // already unioned

  node* set1 = nodes_[setId1];
  node* set2 = nodes_[setId2];

  // Determine which node representing a set has a higher rank. The node with the higher rank is
  // likely to have a bigger subtree so in order to better balance the tree representing the
  // union, the node with the higher rank is made the parent of the one with the lower rank and
  // not the other way around.
  if (set1->rank > set2->rank) {
    set2->parent = set1;
    set1->size = set1->size + set2->size;
  }
  else if (set1->rank < set2->rank) {
    set1->parent = set2;
    set2->size = set2->size + set1->size;
  }
  else // set1->rank == set2->rank
  {
    set2->parent = set1;
    ++set1->rank; // update rank
    set1->size = set1->size + set2->size;
  }

  // Since two sets have fused into one, there is now one less set so update the set count.
  --num_sets_;
}

void vbl_disjoint_sets::add_elements(int num_to_add)
{
  assert(num_to_add >= 0);

  // insert and initialize the specified number of element nodes to the end of the `nodes_` array
  nodes_.insert(nodes_.end(), num_to_add, (node*)NULL);
  for (int i = num_elements_; i < num_elements_ + num_to_add; ++i)
  {
    nodes_[i] = new node();
    nodes_[i]->parent = 0;
    nodes_[i]->index = i;
    nodes_[i]->rank = 0;
    nodes_[i]->size = 1;
  }

  // update element and set counts
  num_elements_ += num_to_add;
  num_sets_ += num_to_add;
}

int vbl_disjoint_sets::num_elements() const
{
  return num_elements_;
}

int vbl_disjoint_sets::num_sets() const
{
  return num_sets_;
}

int vbl_disjoint_sets::size(int set_id) const
{
  assert(set_id < num_elements_);
  return nodes_[set_id]->size;
}
