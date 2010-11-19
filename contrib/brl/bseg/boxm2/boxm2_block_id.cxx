#include <boxm2/boxm2_block_id.h>
//:
// \file

vcl_string boxm2_block_id::to_string(){
  vcl_ostringstream s1;
  s1 << (*this); 
  return s1.str(); 
}


//: scene output stream operator
vcl_ostream& operator <<(vcl_ostream &s, boxm2_block_id& id)
{
  s << "id_" << id.i() << '_' << id.j() << '_' << id.k();
  return s;
}


