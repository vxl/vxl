// This is gel/pop/pop_object.cxx
#include "pop_object.h"
//:
// \file
#include <vcl_iostream.h>


pop_object::pop_object()
{
}

pop_object::pop_object(vcl_vector<pop_parameter*> &params)
{
  params_ = params;
}

//: destructor
pop_object::~pop_object()
{
}

void pop_object::set(vcl_vector<pop_parameter*> &params)
{
  params_ = params;
  update();
}


//: get the parameters
vcl_vector<pop_parameter*> pop_object::get_parameters()
{
  return params_;
}

void pop_object::update()
{
  vcl_cout << "Warning: update should have been called elsewhere\n";
}
