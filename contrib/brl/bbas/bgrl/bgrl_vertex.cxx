// This is brl/bbas/bgrl/bgrl_vertex.cxx
#include "bgrl_vertex.h"
//:
// \file

#include "bgrl_edge.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_set_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//: Constructor
bgrl_vertex::bgrl_vertex()
  : out_edges_(), in_edges_()
{
}


//: Copy Constructor
bgrl_vertex::bgrl_vertex(const bgrl_vertex& vertex)
  : vbl_ref_count()
{
  for (const auto & out_edge : vertex.out_edges_){
    bgrl_edge_sptr edge_copy(out_edge->clone());
    edge_copy->from_ = this;
    out_edges_.insert(edge_copy);
  }
}


//: Strip all of the edges from this vertex
void
bgrl_vertex::strip()
{
  // Iterate over all outgoing edges and remove back links
  for (const auto & out_edge : out_edges_)
  {
    if (out_edge->to_) {
      out_edge->to_->in_edges_.erase(out_edge);
      out_edge->to_ = nullptr;
    }
    out_edge->from_ = nullptr;
  }

  // Clear outgoing edges
  out_edges_.clear();

  // Iterate over all incoming edges and remove back links
  for (const auto & in_edge : in_edges_)
  {
    if (in_edge->from_){
      in_edge->from_->out_edges_.erase(in_edge);
      in_edge->from_ = nullptr;
    }
    in_edge->to_ = nullptr;
  }

  // Clear incoming edges
  in_edges_.clear();
}


//: Remove any edges to or from NULL vertices
bool
bgrl_vertex::purge()
{
  bool retval = false;

  for ( auto itr = out_edges_.begin();
        itr != out_edges_.end(); )
  {
    auto next_itr = itr;
    ++next_itr;
    if (!(*itr)->to_) {
      out_edges_.erase(itr);
      retval = true;
    }
    itr = next_itr;
  }

  for ( auto itr = in_edges_.begin();
        itr != in_edges_.end(); )
  {
    auto next_itr = itr;
    ++next_itr;
    if (!(*itr)->from_) {
      in_edges_.erase(itr);
      retval = true;
    }
    itr = next_itr;
  }

  return retval;
}


//: Add an edge to \p vertex
bgrl_edge_sptr
bgrl_vertex::add_edge_to( const bgrl_vertex_sptr& vertex,
                          const bgrl_edge_sptr& model_edge )
{
  if (!vertex || vertex.ptr() == this)
    return bgrl_edge_sptr(nullptr);

  // verify that this edge is not already present
  for (const auto & out_edge : out_edges_)
    if (out_edge->to_ == vertex)
      return bgrl_edge_sptr(nullptr);

  // add the edge
  bgrl_edge_sptr new_edge;
  if (model_edge)
    new_edge = model_edge->clone();
  else
    new_edge = new bgrl_edge;

  new_edge->from_ = this;
  new_edge->to_ = vertex.ptr();
  this->out_edges_.insert(new_edge);
  vertex->in_edges_.insert(new_edge);

  // initialize the edge
  new_edge->init();

  return new_edge;
}


//: Remove \p vertex from the neighborhood
bool
bgrl_vertex::remove_edge_to( const bgrl_vertex_sptr& vertex )
{
  if (!vertex || vertex.ptr() == this)
    return false;

  for ( auto itr = out_edges_.begin();
        itr != out_edges_.end(); ++itr )
  {
    if ((*itr)->to_ == vertex) {
      if ( vertex->in_edges_.erase(*itr) > 0 ) {
        (*itr)->to_ = nullptr;
        (*itr)->from_ = nullptr;
        out_edges_.erase(itr);
        return true;
      }
    }
  }

  return false;
}


//: Returns an iterator to the beginning of the list of outgoing edges
bgrl_vertex::edge_iterator
bgrl_vertex::begin()
{
  return out_edges_.begin();
}


//: Returns an iterator to the end of the list of outgoing edges
bgrl_vertex::edge_iterator
bgrl_vertex::end()
{
  return out_edges_.end();
}


//: Return a platform independent string identifying the class
std::string
bgrl_vertex::is_a() const
{
  return "bgrl_vertex";
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bgrl_vertex*
bgrl_vertex::clone() const
{
  return new bgrl_vertex(*this);
}


//: Binary save self to stream.
void
bgrl_vertex::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());

  // write the outgoing edges
  vsl_b_write(os, out_edges_);
  // write the incoming edges
  vsl_b_write(os, in_edges_);
}


//: Binary load self from stream.
void
bgrl_vertex::b_read( vsl_b_istream& is )
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    // read the outgoing edges
    out_edges_.clear();
    vsl_b_read(is, out_edges_);
    for (const auto & out_edge : out_edges_)
      out_edge->from_ = this;

    // read the incoming edges
    in_edges_.clear();
    vsl_b_read(is, in_edges_);
    for (const auto & in_edge : in_edges_)
      in_edge->to_ = this;

    break;

  default:
    std::cerr << "I/O ERROR: bgrl_vertex::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
bgrl_vertex::version() const
{
  return 1;
}


//: Print an ascii summary to the stream
void
bgrl_vertex::print_summary( std::ostream& os ) const
{
  os << this->degree() << " degree";
}


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------


//: Allows derived class to be loaded by base-class pointer.
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr);".  This loads derived class
//  objects from the stream, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const bgrl_vertex& v)
{
  vsl_binary_loader<bgrl_vertex>::instance().add(v);
}


//: Print an ASCII summary to the stream
void
vsl_print_summary(std::ostream &os, const bgrl_vertex* v)
{
  os << "bgrl_vertex{ ";
  v->print_summary(os);
  os << " }";
}
