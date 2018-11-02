#include <iostream>
#include <sstream>
#include "bstm_block_id.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: scene output stream operator
std::ostream& operator <<(std::ostream &s, bstm_block_id const& id)
{
  s << "id_" << id.i() << '_' << id.j() << '_' << id.k() << '_' << id.t();
  return s;
}

std::string bstm_block_id::to_string() const
{
  std::ostringstream s1;
  s1 << (*this);
  return s1.str();
}

//: Binary write boxm2_block to stream.
void vsl_b_write(vsl_b_ostream&  /*os*/, bstm_block_id_sptr const&  /*sptr*/) {}

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_block_id_sptr&  /*sptr*/) {}

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bstm_block_id_sptr const&  /*sptr*/) {}
