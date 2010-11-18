#include <boxm2/boxm2_block_id.h>
//:
// \file

//: scene output stream operator
vcl_ostream& operator <<(vcl_ostream &s, boxm2_block_id& id)
{
  s << "block_id." << id.i() << '.' << id.j() << '.' << id.k();
  return s;
}
