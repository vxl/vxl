#ifdef __GNUC__
#pragma implementation
#endif

#include "MovieFileInterface.h"

#include <vcl_iostream.h>

bool MovieFileInterface::verbose = false;

MovieFileInterface::~MovieFileInterface()
{
}

bool MovieFileInterface::GetFrame(int /*frame_index*/, void* /*buffer*/)
{
  cerr << "MovieFileInterface::GetFrame not implemented\n";
  return false;
}

bool MovieFileInterface::GetField(int /*frame_index*/, void* /*buffer*/)
{
  cerr << "MovieFileInterface::GetField not implemented\n";
  return false;
}
