// This is contrib/brl/bseg/bmrf/bmrf_network.cxx
//:
// \file

#include "bmrf_node.h"
#include "bmrf_network.h"


//: Constructor
bmrf_network::bmrf_network()
{
}


//: Returns the number of nodes in the network;
int
bmrf_network::size()
{
  return nodes_.size();
}


//: Returns the probability that the entire network is correct
double
bmrf_network::probability()
{
  // We have not yet determined how to calculate this
  return 0.0;
}


//: Returns all the nodes in frame \param frame
vcl_vector<bmrf_node_sptr>
bmrf_network::nodes_in_frame(int frame) const
{
  return vcl_vector<bmrf_node_sptr>();
}


//: Increment the current node
void
bmrf_network::depth_iterator::next_node()
{
  if (curr_node_.ptr() == NULL) return;
  bmrf_node::neighbor_iterator itr = curr_node_->begin();
  for(; itr != curr_node_->end(); ++itr){
    eval_queue_.push_front((*itr)->to);
  }
  while( visited_.find(eval_queue_.front()) != visited_.end() )
    eval_queue_.pop_front();
  if (eval_queue_.empty())
    curr_node_ = NULL;
  else{
    curr_node_ = eval_queue_.front();
    eval_queue_.pop_front();
    visited_.insert(curr_node_);
  }
}


//: Increment the current node
void
bmrf_network::breadth_iterator::next_node()
{
  if (curr_node_.ptr() == NULL) return;
  bmrf_node::neighbor_iterator itr = curr_node_->begin();
  for(; itr != curr_node_->end(); ++itr){
    eval_queue_.push_back((*itr)->to);
  }
  while( visited_.find(eval_queue_.front()) != visited_.end() )
    eval_queue_.pop_front();
  if (eval_queue_.empty())
    curr_node_ = NULL;
  else{
    curr_node_ = eval_queue_.front();
    eval_queue_.pop_front();
    visited_.insert(curr_node_);
  }
}



