// This is vxl/vil/vil_pyramid.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_pyramid.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vil/vil_resample.h>
#include <vil/vil_memory_image.h>

vil_pyramid::vil_pyramid(vil_image const &I, cache_strategy cs_)
{
  cs = cs_;
  levels.push_back(I);
}

vil_pyramid::~vil_pyramid()
{
}

vil_image vil_pyramid::operator[](unsigned i)
{
  while (i >= levels.size()) {
    vcl_cerr << "making pyramid level " << levels.size() << vcl_endl;
    vil_image I = levels.back();
    I = vil_resample(I, I.width()/2, I.height()/2);
    switch (cs) {
    case none: break;
    case memory: I = vil_memory_image(I /*, "silly hack"*/); break;
    case blocked: /* not implemented yet */
    default: assert(!"not implemented"); break;
    }
    levels.push_back(I);
  }
  return levels[i];
}
