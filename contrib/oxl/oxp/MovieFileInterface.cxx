//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "MovieFileInterface.h"
#endif
//
// Class: MovieFileInterface
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 31 Dec 98
// Modifications:
//   981231 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "MovieFileInterface.h"

#include <vcl/vcl_iostream.h>

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
