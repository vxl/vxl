/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_pyramid"
#endif
#include "vil_pyramid.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>
#include <vil/vil_resample.h>
#include <vil/vil_memory_image.h>

vil_pyramid::vil_pyramid(vil_image const &I, cache_strategy cs_ VCL_DEFAULT_VALUE(memory))
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
    cerr << "making pyramid level " << levels.size() << endl;
    vil_image I = levels.back();
    I = vil_resample(I, I.width()/2, I.height()/2);
    switch (cs) {
    case none: break;
    case memory: I = vil_memory_image(I, "silly hack"); break;
    case blocked: /* not implemented yet */
    default: assert(false); break;
    }
    levels.push_back(I);
  }
  return levels[i];
}
