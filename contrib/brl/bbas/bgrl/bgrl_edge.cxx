// This is brl/bbas/bgrl/bgrl_edge.cxx
#include "bgrl_edge.h"
//:
// \file

#include <vsl/vsl_binary_loader.h>


//: Return a platform independent string identifying the class
vcl_string 
bgrl_edge::is_a() const 
{ 
  return "bgrl_edge"; 
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bgrl_edge* 
bgrl_edge::clone() const
{
  return new bgrl_edge(*this);
}


//: Binary save bgrl_edge to stream.
void
bgrl_edge::b_write( vsl_b_ostream& ) const
{
  // Nothing to write
  // The smart pointer to the edge is written because the smart
  // pointer serialization is needed to serialize the graph
}


//: Binary load bgrl_edge from stream.
void
bgrl_edge::b_read( vsl_b_istream& )
{
  // Nothing to read
  // The smart pointer to the edge is read because the smart
  // pointer serialization is needed to serialize the graph
}


//: Print an ascii summary to the stream
void
bgrl_edge::print_summary( vcl_ostream& os ) const
{
  os << "2 vertices";
}


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------


//: Binary save bgrl_edge \p e to stream.
void
vsl_b_write(vsl_b_ostream &os, const bgrl_edge* e)
{
  if (!e) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    e->b_write(os);
  }
}


//: Binary load bgrl_edge \p e from stream.
void
vsl_b_read(vsl_b_istream &is, bgrl_edge* &e)
{
  delete e;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    e = new bgrl_edge();
    e->b_read(is);
  }
  else
    e = 0;
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
void vsl_add_to_binary_loader(const bgrl_edge& e)
{
  vsl_binary_loader<bgrl_edge>::instance().add(e);
}


//: Print an ASCII summary of a bgrl_edge to the stream
void
vsl_print_summary(vcl_ostream &os, const bgrl_edge* e)
{
  os << "bgrl_edge{ ";
  e->print_summary(os);
  os << " }";
}

