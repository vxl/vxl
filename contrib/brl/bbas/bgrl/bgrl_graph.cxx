// This is brl/bbas/bgrl/bgrl_graph.cxx
#include "bgrl_graph.h"
//:
// \file
#include "bgrl_vertex.h"
#include "bgrl_edge.h"
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_set_io.h>
#include <vsl/vsl_binary_loader.h>
#include <vcl_iostream.h>

//: Constructor
bgrl_graph::bgrl_graph()
{
}

//: Copy Constructor
// \note this provides a deep copy of the graph
bgrl_graph::bgrl_graph(const bgrl_graph& graph)
{
  vcl_map<bgrl_vertex_sptr, bgrl_vertex_sptr> old_to_new;

  // copy vertices and outgoing edges
  for ( vcl_set<bgrl_vertex_sptr>::const_iterator itr = graph.vertices_.begin();
        itr != graph.vertices_.end();  ++itr )
  {
    bgrl_vertex_sptr vertex_copy((*itr)->clone());
    old_to_new[*itr] = vertex_copy;
    vertices_.insert(vertex_copy);
  }

  // link up new edges to new vertices
  for ( vcl_set<bgrl_vertex_sptr>::const_iterator v_itr = vertices_.begin();
        v_itr != vertices_.end();  ++v_itr )
  {
    for ( vcl_set<bgrl_edge_sptr>::const_iterator e_itr = (*v_itr)->out_edges_.begin();
          e_itr != (*v_itr)->out_edges_.end();  ++e_itr )
    {
      vcl_map<bgrl_vertex_sptr, bgrl_vertex_sptr>::iterator find_new = old_to_new.find((*e_itr)->to());
      if ( find_new != old_to_new.end() ){
        (*e_itr)->to_ = find_new->second.ptr();
        find_new->second->in_edges_.insert(*e_itr);
      }
      else
        vcl_cerr << "Error copying graph: vertex not found in graph" << vcl_endl;
    }
  }
}


//: Adds a new vertex to the graph
bool
bgrl_graph::add_vertex(const bgrl_vertex_sptr& vertex)
{
  if (!vertex) return false;

  return vertices_.insert(vertex).second;
}


//: Deletes a vertex in the graph
bool
bgrl_graph::remove_vertex(const bgrl_vertex_sptr& vertex)
{
  if (!vertex) return false;

  if (vertices_.erase(vertex) == 0) return false;

  vertex->strip();

  return true;
}


//: Add an edge between \p v1 and \p v2
bgrl_edge_sptr
bgrl_graph::add_edge( const bgrl_vertex_sptr& v1, const bgrl_vertex_sptr& v2 )
{
  if (!v1 || !v2)
    return 0;
  if ( vertices_.count(v1) == 0 && !this->add_vertex(v1) )
    return 0;
  if ( vertices_.count(v2) == 0 && !this->add_vertex(v2) )
    return 0;

  return v1->add_edge_to(v2);
}


//: Remove an edge between \p v1 and \p v2
bool
bgrl_graph::remove_edge( const bgrl_vertex_sptr& v1, const bgrl_vertex_sptr& v2 )
{
  if (!v1 || !v2) return false;

  return v1->remove_edge_to(v2);
}

//: Remove all edges to NULL vertices and vertex not found in this graph
bool
bgrl_graph::purge()
{
  bool retval = false;

  for ( vertex_iterator v_itr = vertices_.begin();
        v_itr != vertices_.end(); ++v_itr )
  {
    bgrl_vertex_sptr curr_vertex = *v_itr;
    // remove the NULL edges
    retval = curr_vertex->purge() || retval;
    // remove edges to vertices not in this graph
    for ( edge_iterator e_itr = curr_vertex->out_edges_.begin();
          e_itr != curr_vertex->out_edges_.end(); ++e_itr )
    {
      if (vertices_.find((*e_itr)->to()) == vertices_.end()) {
        curr_vertex->remove_edge_to((*e_itr)->to());
        retval = true;
      }
    }
    // remove edges from vertices not in this graph
    for ( edge_iterator e_itr = curr_vertex->in_edges_.begin();
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


//: Return a platform independent string identifying the class
vcl_string 
bgrl_graph::is_a() const 
{ 
  return "bgrl_graph"; 
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bgrl_graph* 
bgrl_graph::clone() const
{
  return new bgrl_graph(*this);
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
  switch (ver)
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


//-----------------------------------------------------------------------------------------
// Iterator functions
//-----------------------------------------------------------------------------------------

//: Constructor
bgrl_graph::iterator::iterator( bgrl_graph_sptr graph, bgrl_search_func_sptr func ) 
 : graph_(graph), search_func_(func), 
   use_internal_(false), internal_(graph->vertices_.begin()) 
{
  if(!func)
    use_internal_ = true;
}


//: Pre-Increment
bgrl_graph::iterator& 
bgrl_graph::iterator::operator++ ()
{ 
  if(use_internal_)
    ++internal_;
  else
    search_func_->next_vertex();
  
  return *this; 
}

//: Post-Increment
bgrl_graph::iterator 
bgrl_graph::iterator::operator++ (int)
{ 
  iterator old(*this);
  ++(*this);
  return old;
}


//: Dereference
bgrl_vertex_sptr 
bgrl_graph::iterator::operator -> () const 
{
  return *(*this);
}


//: Dereference
bgrl_vertex_sptr 
bgrl_graph::iterator::operator * () const 
{ 
  if(use_internal_)
    if(internal_ == this->graph_->vertices_.end())
      return NULL;
    else
      return *internal_;
  else
    return search_func_->curr_vertex();
}


//: Equality comparison
bool 
bgrl_graph::iterator::operator == (const iterator& rhs) const 
{ 
  return *rhs == *(*this); 
}

//: Inequality comparison
bool 
bgrl_graph::iterator::operator != (const iterator& rhs) const 
{ 
  return !(rhs == *this);
}


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------


//: Binary save bgrl_graph to stream.
void
vsl_b_write(vsl_b_ostream &os, const bgrl_graph* g)
{
  if (!g) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    g->b_write(os);
  }
}


//: Binary load bgrl_graph from stream.
void
vsl_b_read(vsl_b_istream &is, bgrl_graph* &g)
{
  delete g;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    g = new bgrl_graph();
    g->b_read(is);
  }
  else
    g = 0;
}


//: Allows derived class to be loaded by base-class pointer.
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr);".  This loads derived class
//  objects from the stream, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const bgrl_graph& g)
{
  vsl_binary_loader<bgrl_graph>::instance().add(g);
}


//: Print an ASCII summary to the stream
void
vsl_print_summary(vcl_ostream &os, const bgrl_graph* g)
{
  os << "bgrl_graph{";
  g->print_summary(os);
  os << '}';
}

