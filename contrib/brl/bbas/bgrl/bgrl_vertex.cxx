// This is brl/bbas/bgrl/bgrl_vertex.cxx
#include "bgrl_vertex.h"
//:
// \file

#include "bgrl_edge.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_set_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//: Constructor
bgrl_vertex::bgrl_vertex()
  : out_edges_(), in_edges_()
{
}


//: Strip all of the edges from this vertex
void
bgrl_vertex::strip()
{
  // Iterate over all outgoing edges and remove back links
  for ( edge_iterator out_itr = out_edges_.begin();
        out_itr != out_edges_.end(); ++out_itr)
  {
    if ((*out_itr)->to_) {
      (*out_itr)->to_->in_edges_.erase(*out_itr);
      (*out_itr)->to_ = NULL;
    }
    (*out_itr)->from_ = NULL;
  }

  // Clear outgoing edges
  out_edges_.clear();

  // Iterate over all incoming edges and remove back links
  for ( edge_iterator in_itr = in_edges_.begin();
        in_itr != in_edges_.end(); ++in_itr)
  {
    if ((*in_itr)->from_){
      (*in_itr)->from_->out_edges_.erase(*in_itr);
      (*in_itr)->from_ = NULL;
    }
    (*in_itr)->to_ = NULL;
  }

  // Clear incoming edges
  in_edges_.clear();
}


//: Remove any edges to or from NULL vertices
bool
bgrl_vertex::purge()
{
  bool retval = false;

  for ( edge_iterator itr = out_edges_.begin();
        itr != out_edges_.end(); )
  {
    edge_iterator next_itr = itr;
    ++next_itr;
    if (!(*itr)->to_) {
      out_edges_.erase(itr);
      retval = true;
    }
    itr = next_itr;
  }

  for ( edge_iterator itr = in_edges_.begin();
        itr != in_edges_.end(); )
  {
    edge_iterator next_itr = itr;
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
bgrl_vertex::add_edge_to( const bgrl_vertex_sptr& vertex )
{
  if (!vertex || vertex.ptr() == this)
    return bgrl_edge_sptr(NULL);

  // verify that this edge is not already present
  for ( edge_iterator itr = out_edges_.begin();
        itr != out_edges_.end(); ++itr )
    if ((*itr)->to() == vertex)
      return bgrl_edge_sptr(NULL);

  // add the edge
  bgrl_edge_sptr new_edge = new bgrl_edge(this, vertex);
  out_edges_.insert(new_edge);
  vertex->in_edges_.insert(new_edge);

  return new_edge;
}


//: Remove \p vertex from the neighborhood
bool
bgrl_vertex::remove_edge_to( const bgrl_vertex_sptr& vertex )
{
  if (!vertex || vertex.ptr() == this)
    return false;

  for ( edge_iterator itr = out_edges_.begin();
        itr != out_edges_.end(); ++itr )
  {
    if ((*itr)->to() == vertex) {
      if ( vertex->in_edges_.erase(*itr) > 0 ) {
        (*itr)->to_ = NULL;
        (*itr)->from_ = NULL;
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
  switch(ver)
  {
  case 1:
    // read the outgoing edges
    out_edges_.clear();
    vsl_b_read(is, out_edges_);
    for ( edge_iterator itr = out_edges_.begin();
          itr != out_edges_.end(); ++itr )
      (*itr)->from_ = this;

    // read the incoming edges
    in_edges_.clear();
    vsl_b_read(is, in_edges_);
    for ( edge_iterator itr = in_edges_.begin();
          itr != in_edges_.end(); ++itr )
      (*itr)->to_ = this;

    break;

  default:
    vcl_cerr << "I/O ERROR: bgrl_vertex::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
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
bgrl_vertex::print_summary( vcl_ostream& os ) const
{
  os << this->degree() << " degree";
}


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------


//: Binary save bgrl_vertex to stream.
void
vsl_b_write(vsl_b_ostream &os, const bgrl_vertex* v)
{
  if (!v) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    v->b_write(os);
  }
}


//: Binary load bgrl_vertex from stream.
void
vsl_b_read(vsl_b_istream &is, bgrl_vertex* &v)
{
  delete v;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    v = new bgrl_vertex();
    v->b_read(is);
  }
  else
    v = 0;
}


//: Print an ASCII summary to the stream
void
vsl_print_summary(vcl_ostream &os, bgrl_vertex_sptr v)
{
  os << "bgrl_vertex{ ";
  v->print_summary(os);
  os << " }";
}

