// This is contrib/brl/bseg/bmrf/bmrf_network.cxx
//:
// \file

#include "bmrf_node.h"
#include "bmrf_network.h"
#include <vbl/io/vbl_io_smart_ptr.h>


//: Constructor
bmrf_network::bmrf_network()
{
}


//: Adds a new to the network
bool
bmrf_network::add_node(const bmrf_node_sptr& node)
{
  if(!node.ptr()) return false;
  if(!node->epi_seg().ptr()) return false;
  if(nodes_.find(node->epi_seg().ptr()) == nodes_.end()) return false;

  nodes_[node->epi_seg().ptr()] = node;
  return true;
}


//: Deletes a node in the network
bool
bmrf_network::delete_node(const bmrf_node_sptr& node)
{
  if(!node.ptr()) return false;
  if(!node->epi_seg().ptr()) return false;
  node_map::iterator itr = nodes_.find(node->epi_seg().ptr());
  if(itr == nodes_.end()) return false;

  node->strip();
  nodes_.erase(itr);
  return true;
}
  

//: Look up the node corresponding to an epi-segment
bmrf_node_sptr
bmrf_network::seg_to_node(const bmrf_epi_seg_sptr& seg) const
{
  node_map::const_iterator itr = nodes_.find(seg.ptr());
  if(itr == nodes_.end())
    return bmrf_node_sptr(NULL);

  return itr->second;
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


//: Binary save self to stream.
void
bmrf_network::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());
 
  // write the number of nodes
  vsl_b_write(os, nodes_.size());
  // write all the nodes
  node_map::const_iterator itr = nodes_.begin();
  for(; itr != nodes_.end(); ++itr){
    vsl_b_write(os, itr->second);
  }
}


//: Binary load self from stream.
void
bmrf_network::b_read( vsl_b_istream& is )
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      nodes_.clear();
    
      int num_nodes;
      vsl_b_read(is, num_nodes);
      for(int n=0; n<num_nodes; ++n){
        bmrf_node_sptr node;
        vsl_b_read(is, node);
        nodes_[node->epi_seg().ptr()] = node;
      }
    }
    break;

  default:
    vcl_cerr << "I/O ERROR: bmrf_network::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Print an ascii summary to the stream
void
bmrf_network::print_summary( vcl_ostream& os ) const
{
  os << "bmrf_network{ " << nodes_.size() << " nodes }";
}


//: Return IO version number;
short
bmrf_network::version(  ) const
{
  return 1;
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



