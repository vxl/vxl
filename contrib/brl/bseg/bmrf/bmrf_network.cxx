// This is brl/bseg/bmrf/bmrf_network.cxx
#include "bmrf_network.h"
//:
// \file

#include "bmrf_node.h"
#include <vbl/io/vbl_io_smart_ptr.h>


//: Constructor
bmrf_network::bmrf_network()
{
}


//: Adds a new to the network
bool
bmrf_network::add_node(const bmrf_node_sptr& node)
{
  if (!node.ptr()) return false;
  if (!node->epi_seg().ptr()) return false;
  if (node_from_seg_.find(node->epi_seg().ptr()) != node_from_seg_.end()) return false;

  node_from_seg_[node->epi_seg().ptr()] = node;
  nodes_from_frame_[node->frame_num()][node->epi_seg().ptr()] = node;
  return true;
}


//: Deletes a node in the network
bool
bmrf_network::remove_node(const bmrf_node_sptr& node)
{
  if (!node.ptr()) return false;
  if (!node->epi_seg().ptr()) return false;
  seg_node_map::iterator itr = node_from_seg_.find(node->epi_seg().ptr());
  if (itr == node_from_seg_.end()) return false;

  node->strip();
  node_from_seg_.erase(itr);
  itr = nodes_from_frame_[node->frame_num()].find(node->epi_seg().ptr());
  nodes_from_frame_[node->frame_num()].erase(itr);
  return true;
}


//: Add an arc between \p n1 and \p n2 of type \p type
bool
bmrf_network::add_arc( const bmrf_node_sptr& n1, const bmrf_node_sptr& n2, neighbor_type type )
{
  if (!n1.ptr() || !n2.ptr()) return false;
  if (!n1->epi_seg().ptr() || !n2->epi_seg().ptr()) return false;

  seg_node_map::iterator itr = node_from_seg_.find(n1->epi_seg().ptr());
  if (itr == node_from_seg_.end())
    if ( !this->add_node(n1) )
      return false;
  itr = node_from_seg_.find(n2->epi_seg().ptr());
  if (itr == node_from_seg_.end())
    if ( !this->add_node(n2) )
      return false;

  return n1->add_neighbor(n2.ptr(), type);
}


//: Add an arc between \p n1 and \p n2 of type \p type
bool
bmrf_network::remove_arc( const bmrf_node_sptr& n1, const bmrf_node_sptr& n2, neighbor_type type )
{
  if (!n1.ptr() || !n2.ptr()) return false;

  return n1->remove_neighbor(n2.ptr(), type);
}

//: Remove all arcs to NULL nodes and node not found in this network
bool
bmrf_network::purge()
{
  bool retval = false;
  seg_node_map::const_iterator n_itr = node_from_seg_.begin();
  for (; n_itr != node_from_seg_.end(); ++n_itr) {
    bmrf_node_sptr curr_node = n_itr->second;
    retval = curr_node->purge() || retval; // remove the NULL arcs
    bmrf_node::arc_iterator a_itr = curr_node->begin();
    for (; a_itr != curr_node->end(); ++a_itr) {
      if (node_from_seg_.find((*a_itr)->to()->epi_seg().ptr()) == node_from_seg_.end()) {
        curr_node->remove_neighbor((*a_itr)->to().ptr());
        retval = true;
      }
    }
  }
  return retval;
}


//: Look up the node corresponding to an epi-segment
bmrf_node_sptr
bmrf_network::seg_to_node(const bmrf_epi_seg_sptr& seg, int frame) const
{
  seg_node_map::const_iterator itr;
  if (frame < 0) {
    itr = node_from_seg_.find(seg.ptr());
    if (itr == node_from_seg_.end())
      return bmrf_node_sptr(NULL);
  }
  else {
    frame_node_map::const_iterator map_itr = nodes_from_frame_.find(frame);
    if ( map_itr == nodes_from_frame_.end())
      return bmrf_node_sptr(NULL);
    itr = map_itr->second.find(seg.ptr());
    if (itr == map_itr->second.end())
      return bmrf_node_sptr(NULL);
  }

  return itr->second;
}


//: Returns the number of nodes in the network;
int
bmrf_network::size(int frame)
{
  if (frame < 0)
    return node_from_seg_.size();
  else
    return nodes_from_frame_[frame].size();
}


//: Returns the probability that the entire network is correct
double
bmrf_network::probability()
{
  // We have not yet determined how to calculate this
  return 0.0;
}


//: Set the epipole for frame \p frame
void
bmrf_network::set_epipole(const bmrf_epipole& epipole, int frame)
{
  // for now we assume that there is only one epipole for the
  // entire sequence.  This function should be update if this
  // assumption changes
  if (epipoles_.empty())
    epipoles_.resize(1);
  epipoles_[0] = epipole;
}


//: Access the epipole for frame \p frame
const bmrf_epipole&
bmrf_network::epipole(int frame) const
{
  // for now we assume that there is only one epipole for the
  // entire sequence.  This function should be update if this
  // assumption changes
  return epipoles_.front();
}


//: Returns the beginning const iterator to the nodes in frame \p frame
bmrf_network::seg_node_map::const_iterator
bmrf_network::begin( int frame ) const
{
  if (frame < 0) return node_from_seg_.begin();

  frame_node_map::const_iterator itr = nodes_from_frame_.find(frame);
  if (itr != nodes_from_frame_.end())
    return itr->second.begin();
  else
   return node_from_seg_.begin();
}


//: Returns the end const iterator to the nodes in frame \p frame
bmrf_network::seg_node_map::const_iterator
bmrf_network::end( int frame ) const
{
  if (frame < 0) return node_from_seg_.end();

  frame_node_map::const_iterator itr = nodes_from_frame_.find(frame);
  if (itr != nodes_from_frame_.end())
    return itr->second.end();
  else
   return node_from_seg_.end();
}


//: Binary save self to stream.
void
bmrf_network::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());
  // write the number of nodes
  vsl_b_write(os, (unsigned int)node_from_seg_.size());
  // write all the nodes
  seg_node_map::const_iterator itr = node_from_seg_.begin();

  for (; itr != node_from_seg_.end(); ++itr) {
    vsl_b_write(os, itr->second);
  }

  // write the number of epipoles
  vsl_b_write(os, (unsigned int)epipoles_.size());
  // write all the epipoles
  for ( vcl_vector<bmrf_epipole>::const_iterator itr = epipoles_.begin();
       itr != epipoles_.end(); ++itr ) {
    vsl_b_write(os, itr->location().x());
    vsl_b_write(os, itr->location().y());
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
      node_from_seg_.clear();

      unsigned int num_nodes;
      vsl_b_read(is, num_nodes);
      for (unsigned int n=0; n<num_nodes; ++n) {
        bmrf_node_sptr node;
        vsl_b_read(is, node);
        node_from_seg_[node->epi_seg().ptr()] = node;
        nodes_from_frame_[node->frame_num()][node->epi_seg().ptr()] = node;
      }

      epipoles_.clear();
      unsigned int num_epipoles;
      vsl_b_read(is, num_epipoles);
      for (unsigned int n=0; n<num_epipoles; ++n) {
        double x,y;
        vsl_b_read(is, x);
        vsl_b_read(is, y);
        epipoles_.push_back(bmrf_epipole(x, y));
      }
    }
    if (this->purge())
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
  os << ' ' << node_from_seg_.size() << " nodes in "<< nodes_from_frame_.size() <<" frames ";
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
  bmrf_node::arc_iterator itr = curr_node_->end();
  for (--itr; itr != curr_node_->end(); --itr) {
    if ( visited_.find((*itr)->to().ptr()) == visited_.end() )
      eval_queue_.push_front((*itr)->to().ptr());
  }
  while ( !eval_queue_.empty() && visited_.find(eval_queue_.front()) != visited_.end() )
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
  for (; itr != curr_node_->end(); ++itr) {
    if ( visited_.find((*itr)->to().ptr()) == visited_.end() )
      eval_queue_.push_back((*itr)->to().ptr());
  }
  while ( !eval_queue_.empty() && visited_.find(eval_queue_.front()) != visited_.end() )
    eval_queue_.pop_front();
  if (eval_queue_.empty())
    curr_node_ = NULL;
  else {
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
  if (n==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
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
  if (not_null_ptr) {
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
  os << '}';
}

