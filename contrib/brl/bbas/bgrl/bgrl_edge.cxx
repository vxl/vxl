// This is brl/bbas/bgrl/bgrl_edge.cxx
//:
// \file

#include "bgrl_edge.h"



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


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------


//: Binary save bgrl_edge \p e to stream.
void
vsl_b_write(vsl_b_ostream &os, const bgrl_edge* e)
{
  if (e==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
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


//: Print an ASCII summary of a bgrl_edge to the stream (NYI)
void
vsl_print_summary(vcl_ostream &os, const bgrl_edge* e)
{
  os << "bgrl_edge{}";
}

