#include <boxm2/boxm2_block_id.h>
//:
// \file

//: scene output stream operator
vcl_ostream& operator <<(vcl_ostream &s, boxm2_block_id& id)
{
  s << "block_id." << id.i() << '.' << id.j() << '.' << id.k();
  return s;
}

vcl_string boxm2_block_id::to_string(){
  vcl_ostringstream s1;
  s1 << (*this); 
  return s1.str(); 
}
