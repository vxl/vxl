// This is gel/pop/pop_parameter.cxx
#include "pop_parameter.h"
//:
// \file

pop_parameter::pop_parameter()
{
  // assume changeable
  is_changeable_ = true;

  // default values
  value_ = 0;
  std_ = 1.0;
}

//: destructor
pop_parameter::~pop_parameter()
{
}
