// This is gel/pop/pop_geometric_object.cxx
#include <iostream>
#include "pop_geometric_object.h"
//:
// \file
#include <pop/pop_edge.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: constructor
pop_geometric_object::pop_geometric_object(pop_vertex *coordinate_system)
{
  coordinate_system_ = coordinate_system;
}

//: constructor
pop_geometric_object::pop_geometric_object(pop_vertex *coordinate_system, std::vector<pop_parameter*> &param ):
  pop_object(param)
{
  coordinate_system_ = coordinate_system;
}

//: destructor
pop_geometric_object::~pop_geometric_object()
{
}

//: we should be able to transform to another coordinate system
pop_geometric_object* pop_geometric_object::transform(pop_vertex *destination)
{
  // step one get a list of transformations
  if (!coordinate_system_)
    return 0;

  std::list<pop_edge*> transforms;
  if (!(coordinate_system_->find_path(destination,transforms)))
    return 0;

  // transform to the end of the list

  std::list<pop_edge*>::iterator ei;
  pop_geometric_object* go_old = this;
  pop_geometric_object* go = this;

  for (ei=transforms.begin();ei!=transforms.end();ei++)
  {
    go = (*ei)->get_transform()->transform(go);

    if (!go)
      std::cout << "Warning: pop_geometric_object::transform could not transform object\n";

    // delete intermediary results should be done with smart pointers TODO

    if (go_old != this)
      delete go_old;
    go_old = go;
  }

  return go;
}

double pop_geometric_object::cost(pop_geometric_object *other)
{
  // should be delegated to another object
  return 0;
}
