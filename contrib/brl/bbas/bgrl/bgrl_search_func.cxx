// This is brl/bbas/bgrl/bgrl_search_func.cxx
#include "bgrl_search_func.h"
//:
// \file

#include "bgrl_vertex.h"
#include "bgrl_edge.h"


//: Breadth First Search
bgrl_edge_sptr
bgrl_breadth_search::next_vertex()
{
  if (!curr_vertex_)
    return VXL_NULLPTR;

  for ( bgrl_vertex::edge_iterator itr = curr_vertex_->begin();
        itr != curr_vertex_->end(); ++itr )
  {
    if ( visited_.find((*itr)->to()) == visited_.end() )
      eval_queue_.push_back(*itr);
  }
  while ( !eval_queue_.empty() && visited_.find(eval_queue_.front()->to()) != visited_.end() )
    eval_queue_.pop_front();

  if (eval_queue_.empty()){
    curr_vertex_ = VXL_NULLPTR;
    return VXL_NULLPTR;
  }
  else {
    bgrl_edge_sptr next = eval_queue_.front();
    curr_vertex_ = next->to();
    eval_queue_.pop_front();
    visited_.insert(curr_vertex_);
    return next;
  }
}


//=========================================================================

//: Depth first search
bgrl_edge_sptr
bgrl_depth_search::next_vertex()
{
  if (!curr_vertex_)
    return VXL_NULLPTR;

  for ( bgrl_vertex::edge_iterator itr = curr_vertex_->begin();
        itr != curr_vertex_->end(); ++itr )
  {
    if ( visited_.find((*itr)->to()) == visited_.end() )
      eval_queue_.push_front(*itr);
  }
  while ( !eval_queue_.empty() && visited_.find(eval_queue_.front()->to()) != visited_.end() )
    eval_queue_.pop_front();

  if (eval_queue_.empty()){
    curr_vertex_ = VXL_NULLPTR;
    return VXL_NULLPTR;
  }
  else {
    bgrl_edge_sptr next = eval_queue_.front();
    curr_vertex_ = next->to();
    eval_queue_.pop_front();
    visited_.insert(curr_vertex_);
    return next;
  }
}
