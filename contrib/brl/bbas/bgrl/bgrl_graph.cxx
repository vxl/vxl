// This is brl/bbas/bgrl/bgrl_graph.cxx
//:
// \file

#include "bgrl_graph.h"
#include "bgrl_vertex.h"
#include "bgrl_edge.h"
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_set_io.h>
#include <vcl_algorithm.h>


//: Constructor
bgrl_graph::bgrl_graph()
{
}


//: Adds a new vertex to the graph
bool
bgrl_graph::add_vertex(const bgrl_vertex_sptr& vertex)
{
  if (!vertex.ptr()) return false;

  return vertices_.insert(vertex).second;
}


//: Deletes a vertex in the graph
bool
bgrl_graph::remove_vertex(const bgrl_vertex_sptr& vertex)
{
  if (!vertex.ptr()) return false;

  if (vertices_.erase(vertex) == 0) return false;

  vertex->strip();
  
  return true;
}


//: Add an edge between \p v1 and \p v2 
bgrl_edge_sptr
bgrl_graph::add_edge( const bgrl_vertex_sptr& v1, const bgrl_vertex_sptr& v2 )
{
  if (!v1.ptr() || !v2.ptr()) return false;
 
  if ( vertices_.count(v1) == 0 && !this->add_vertex(v1) )
    return bgrl_edge_sptr(NULL);
  if ( vertices_.count(v2) == 0 && !this->add_vertex(v2) )
    return bgrl_edge_sptr(NULL);

  return v1->add_edge_to(v2);
}


//: Remove an edge between \p v1 and \p v2 
bool
bgrl_graph::remove_edge( const bgrl_vertex_sptr& v1, const bgrl_vertex_sptr& v2 )
{
  if (!v1.ptr() || !v2.ptr()) return false;

  return v1->remove_edge_to(v2);
}

//: Remove all edges to NULL vertices and vertex not found in this graph
bool
bgrl_graph::purge()
{
  bool retval = false;
  
  for( vertex_iterator v_itr = vertices_.begin();
       v_itr != vertices_.end(); ++v_itr ) 
  {
    bgrl_vertex_sptr curr_vertex = *v_itr;
    // remove the NULL edges
    retval = curr_vertex->purge() || retval;
    // remove edges to vertices not in this graph
    for( edge_iterator e_itr = curr_vertex->out_edges_.begin(); 
         e_itr != curr_vertex->out_edges_.end(); ++e_itr ) 
    {
      if (vertices_.find((*e_itr)->to()) == vertices_.end()) {
        curr_vertex->remove_edge_to((*e_itr)->to());
        retval = true;
      }
    }
    // remove edges from vertices not in this graph
    for( edge_iterator e_itr = curr_vertex->in_edges_.begin(); 
         e_itr != curr_vertex->in_edges_.end(); ++e_itr ) 
    {
      if (vertices_.find((*e_itr)->from()) == vertices_.end()) {
        (*e_itr)->from()->remove_edge_to(curr_vertex);
        retval = true;
      }
    }
  }
  return retval;
}


//: Returns the number of vertices in the graph;
int
bgrl_graph::size() const
{
  return vertices_.size();
}


//: Returns the beginning const iterator to the vertices in frame \p frame
bgrl_graph::vertex_iterator
bgrl_graph::begin()
{
  return vertices_.begin();
}


//: Returns the end const iterator to the vertices in frame \p frame
bgrl_graph::vertex_iterator
bgrl_graph::end()
{
  return vertices_.end();
}


//: Binary save self to stream.
void
bgrl_graph::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());

  // write the vertices
  vsl_b_write(os, vertices_);
}


//: Binary load self from stream.
void
bgrl_graph::b_read( vsl_b_istream& is )
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    vsl_b_read(is, vertices_);
    
    if (this->purge())
      vcl_cerr << "I/O WARNING: bgrl_graph::b_read(vsl_b_istream&)\n"
               << "             It is likely that the graph object is corrupt.\n"
               << "             Invalid edges have been purged.\n";
    break;

  default:
    vcl_cerr << "I/O ERROR: bgrl_graph::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Print an ascii summary to the stream
void
bgrl_graph::print_summary( vcl_ostream& os ) const
{
  os << this->size() << " vertices";
}


//: Return IO version number;
short
bgrl_graph::version(  ) const
{
  return 1;
}


//: Increment the current vertex
void
bgrl_graph::depth_iterator::next_vertex()
{
  if (curr_vertex_.ptr() == NULL) return;
  for ( edge_iterator itr = curr_vertex_->begin();
        itr != curr_vertex_->end(); ++itr ) 
  {
    if ( visited_.find((*itr)->to()) == visited_.end() )
      eval_queue_.push_front((*itr)->to());
  }
  while ( !eval_queue_.empty() && visited_.find(eval_queue_.front()) != visited_.end() )
    eval_queue_.pop_front();
  if (eval_queue_.empty())
    curr_vertex_ = NULL;
  else{
    curr_vertex_ = eval_queue_.front();
    eval_queue_.pop_front();
    visited_.insert(curr_vertex_);
  }
}


//: Increment the current vertex
void
bgrl_graph::breadth_iterator::next_vertex()
{
  if (curr_vertex_.ptr() == NULL) return;
  for ( edge_iterator itr = curr_vertex_->begin();
        itr != curr_vertex_->end(); ++itr ) 
  {
    if ( visited_.find((*itr)->to()) == visited_.end() )
      eval_queue_.push_back((*itr)->to());
  }
  while ( !eval_queue_.empty() && visited_.find(eval_queue_.front()) != visited_.end() )
    eval_queue_.pop_front();
  if (eval_queue_.empty())
    curr_vertex_ = NULL;
  else {
    curr_vertex_ = eval_queue_.front();
    eval_queue_.pop_front();
    visited_.insert(curr_vertex_);
  }
}


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------


//: Binary save bgrl_graph to stream.
void
vsl_b_write(vsl_b_ostream &os, const bgrl_graph* n)
{
  if (n==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    n->b_write(os);
  }
}


//: Binary load bgrl_graph from stream.
void
vsl_b_read(vsl_b_istream &is, bgrl_graph* &n)
{
  delete n;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    n = new bgrl_graph();
    n->b_read(is);
  }
  else
    n = 0;
}


//: Print an ASCII summary to the stream
void
vsl_print_summary(vcl_ostream &os, const bgrl_graph* n)
{
  os << "bgrl_graph{";
  n->print_summary(os);
  os << '}';
}

