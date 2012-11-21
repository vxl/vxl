#include "bstm_block_id.h"
//:
// \file
#include <vcl_sstream.h>

//: scene output stream operator
vcl_ostream& operator <<(vcl_ostream &s, bstm_block_id const& id)
{
  s << "id_" << id.i() << '_' << id.j() << '_' << id.k() << '_' << id.t();
  return s;
}

vcl_string bstm_block_id::to_string() const
{
  vcl_ostringstream s1;
  s1 << (*this);
  return s1.str();
}

//: Binary write boxm2_block to stream.
void vsl_b_write(vsl_b_ostream& os, bstm_block_id_sptr const& sptr) {}

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, bstm_block_id_sptr& sptr) {}

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, bstm_block_id_sptr const& sptr) {}
