// This is brl/bbas/imesh/algo/imesh_kd_tree.hxx
#ifndef imesh_kd_tree_hxx_
#define imesh_kd_tree_hxx_
//:
// \file
// \brief A KD-Tree template code
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date June 3, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <algorithm>
#include <limits>
#include "imesh_kd_tree.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//: returns the index of the closest leaf node
// The functor \a dist computes the distance between a point and leaf index
// \param dists (if specified) returns a vector of all explored nodes
//        and the closest square distance found so far
template <class F>
unsigned int
imesh_closest_index(const vgl_point_3d<double>& query,
                    const std::unique_ptr<imesh_kd_tree_node>& kd_root,
                    F dist,
                    std::vector<imesh_kd_tree_queue_entry>* dists = nullptr)
{
  // find the root leaves containing the query point
  std::vector<imesh_kd_tree_queue_entry> leaf_queue, internal_queue;
  imesh_kd_tree_traverse(query,kd_root,leaf_queue,internal_queue);
  assert(!leaf_queue.empty());
  std::make_heap( leaf_queue.begin(), leaf_queue.end() );
  std::make_heap( internal_queue.begin(), internal_queue.end() );

  double closest_dist2 = std::numeric_limits<double>::infinity();
  unsigned int closest_ind = 0;

  if (dists)
    dists->clear();


  // find the closest of the contained leaves
  std::pop_heap( leaf_queue.begin(), leaf_queue.end() );
  std::vector<imesh_kd_tree_queue_entry>::iterator back = leaf_queue.end();
  --back;
  bool break_early = true;
  while (back->val_ < closest_dist2)
  {
    double new_dist2 = dist(query,back->node_->index_);
    new_dist2 *= new_dist2;
    if (dists)
      dists->push_back(imesh_kd_tree_queue_entry(new_dist2,back->node_));
    if (new_dist2 < closest_dist2) {
      closest_dist2 = new_dist2;
      closest_ind = back->node_->index_;
    }
    if (leaf_queue.begin() == back) {
      break_early = false;
      break;
    }
    std::pop_heap( leaf_queue.begin(), back );
    --back;
  }
  // add the unexplored leaves
  if (dists && break_early) {
    ++back;
    std::vector<imesh_kd_tree_queue_entry>::iterator itr = leaf_queue.begin();
    for (; itr != back; ++itr)
      dists->push_back(*itr);
  }
  assert(!dists || dists->size() == leaf_queue.size());


  // check for other closer internal nodes
  std::pop_heap( internal_queue.begin(), internal_queue.end() );
  while (!internal_queue.empty() && internal_queue.back().val_ < closest_dist2)
  {
    imesh_kd_tree_node* current = internal_queue.back().node_;
    internal_queue.pop_back();

    if (current->is_leaf())
    {
      double new_dist2 = dist(query,current->index_);
      new_dist2 *= new_dist2;
      if (dists)
        dists->push_back(imesh_kd_tree_queue_entry(new_dist2,current));
      if (new_dist2 < closest_dist2) {
        closest_dist2 = new_dist2;
        closest_ind = current->index_;
      }
    }
    else
    {
      double left_dist2 = imesh_min_sq_dist(query,current->left_->inner_box_);
      if (left_dist2 < closest_dist2) {
        internal_queue.emplace_back(left_dist2,
                                                           current->left_.get());
        std::push_heap(internal_queue.begin(), internal_queue.end());
      }
      else if (dists)
        dists->push_back(imesh_kd_tree_queue_entry(left_dist2,current->left_.get()));

      double right_dist2 = imesh_min_sq_dist(query,current->right_->inner_box_);
      if (right_dist2 < closest_dist2) {
        internal_queue.emplace_back(right_dist2,
                                                           current->right_.get());
        std::push_heap(internal_queue.begin(), internal_queue.end());
      }
      else if (dists)
        dists->push_back(imesh_kd_tree_queue_entry(right_dist2,current->right_.get()));
    }

    if (!internal_queue.empty())
      std::pop_heap( internal_queue.begin(), internal_queue.end() );
  }
  if (dists) {
    std::vector<imesh_kd_tree_queue_entry>::iterator itr = internal_queue.begin();
    for (; itr != internal_queue.end(); ++itr)
      dists->push_back(*itr);
  }

  return closest_ind;
}


#endif // imesh_kd_tree_hxx_
