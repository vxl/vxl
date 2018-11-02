// This is gel/pop/pop_object.cxx
#include <iostream>
#include "pop_object.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void pop_object::update()
{
  std::cout << "Warning: update should have been called elsewhere\n";
}
