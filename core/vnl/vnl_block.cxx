// This is core/vnl/vnl_block.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vnl_block.h"

#include <vcl_cassert.h>
#include <vcl_compiler.h>
#include <iostream>

void vnl_block_raise_exception(char const *FILE, int LINE, char const *why)
{
  std::cerr << FILE << ":" << LINE << ": " << why << std::endl;
  assert(!"raise_exeption() called");
  // throw;
}
