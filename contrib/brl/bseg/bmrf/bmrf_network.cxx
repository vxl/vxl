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
  if(nodes_.find(node->epi_seg().ptr()) != nodes_.end()) return false;

  nodes_[node->epi_seg().ptr()] = node;
  return true;
}


//: Deletes a node in the network
bool
bmrf_network::remove_node(const bmrf_node_sptr& node)
{
  if(!node.ptr()) return false;
  if(!node->epi_seg().ptr()) return false;
  node_map::iterator itr = nodes_.find(node->epi_seg().ptr());
  if(itr == nodes_.end()) return false;

  node->strip();
  nodes_.erase(itr);
  return true;
}


//: Add an arc between \param n1 and \param n2 of type \param type
bool
bmrf_network::add_arc( const bmrf_node_sptr& n1, const bmrf_node_sptr& n2, neighbor_type type )
{
  if(!n1.ptr() || !n2.ptr()) return false;
  if(!n1->epi_seg().ptr() || !n2->epi_seg().ptr()) return false;
  
  node_map::iterator itr = nodes_.find(n1->epi_seg().ptr());
  if(itr == nodes_.end())
    if( !this->add_node(n1) )
      return false;
  itr = nodes_.find(n2->epi_seg().ptr());
  if(itr == nodes_.end())
    if( !this->add_node(n2) )
      return false;
      
  return n1->add_neighbor(n2.ptr(), type);
}


//: Add an arc between \param n1 and \param n2 of type \param type
bool
bmrf_network::remove_arc( const bmrf_node_sptr& n1, const bmrf_node_sptr& n2, neighbor_type type )
{
  if(!n1.ptr() || !n2.ptr()) return false;
      
  return n1->remove_neighbor(n2.ptr(), type);
}

//: Remove all arcs to NULL nodes and node not found in this network
bool
bmrf_network::purge()
{
  bool retval = false;
  node_map::const_iterator n_itr = nodes_.begin();
  for(; n_itr != nodes_.end(); ++n_itr){
    bmrf_node_sptr curr_node = n_itr->second;
    retval = curr_node->purge() || retval; // remove the NULL arcs
    bmrf_node::arc_iterator a_itr = curr_node->begin();
    for(; a_itr != curr_node->end(); ++a_itr){
      if(nodes_.find((*a_itr)->to->epi_seg().ptr()) == nodes_.end()){
        curr_node->remove_neighbor((*a_itr)->to);
        retval = true;
      }
    }
  }
  return retval;
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
    if(this->purge())
      vcl_cerr << "I/O WARNING: bmrf_network::b_read(vsl_b_istream&)\n"
               << "             It is likely that the network object is corrupt.\n"
               << "             Invalid arcs have been purged.\n";
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
  os << " " << nodes_.size() << " nodes ";
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
  bmrf_node::arc_iterator itr = curr_node_->begin();
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
  bmrf_node::arc_iterator itr = curr_node_->begin();
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


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------


//: Binary save bmrf_network to stream.
void
vsl_b_write(vsl_b_ostream &os, const bmrf_network* n)
{
  if (n==0){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    n->b_write(os);
  }
}


//: Binary load bmrf_network from stream.
void
vsl_b_read(vsl_b_istream &is, bmrf_network* &n)
{
  delete n;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr){
    n = new bmrf_network();
    n->b_read(is);
  }
  else
    n = 0;
}


//: Print an ASCII summary to the stream
void
vsl_print_summary(vcl_ostream &os, const bmrf_network* n)
{
  os << "bmrf_network{";
  n->print_summary(os);
  os << "}";
}

