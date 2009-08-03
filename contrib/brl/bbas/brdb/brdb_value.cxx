// This is brl/bbas/brdb/brdb_value.cxx
#include "brdb_value.h"
//:
// \file
// \author updated by Yong Zhao
// \date Apr 4th, 2007
//
// make it work with the database initially based on Matt's sketch

//: Return a reference to the global registry of database value classes
vcl_map<vcl_string, const brdb_value*> &
brdb_value::mut_registry()
{
  static vcl_map<vcl_string, const brdb_value*> reg;
  return reg;
}

//: The Constructor for this struct registers an exemplar database value object with the base class
brdb_value::registrar::registrar(const brdb_value* exemplar)
{
  vcl_string type = exemplar->is_a();
  //vcl_cout << "registering " << type << vcl_endl;
  // register with the base class
  brdb_value::mut_registry()[type] = exemplar;
}
