// This is brl/bbas/vidl2/vidl2_frame.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   13 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_frame.h"
#include <vcl_cassert.h>

//-----------------------------------------------------------------------------

//: Decrement reference count
void
vidl2_frame::unref()
{
  assert (ref_count_ >0);
  ref_count_--;
  if (ref_count_==0)
  {
    delete this;
  }
}


