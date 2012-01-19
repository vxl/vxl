#include "boxm2_block_id.h"
//:
// \file
#include <vcl_sstream.h>

vcl_string boxm2_block_id::to_string() const
{
  vcl_ostringstream s1;
  s1 << (*this);
  return s1.str();
}


//: scene output stream operator
vcl_ostream& operator <<(vcl_ostream &s, boxm2_block_id const& id) 
{
  s << "id_" << id.i() << '_' << id.j() << '_' << id.k();
  return s;
}


//: Binary write boxm2_block to stream.
void vsl_b_write(vsl_b_ostream& os, boxm2_block_id_sptr const& sptr) {}

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block_id_sptr& sptr) {}

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block_id_sptr const& sptr) {}
