// This is gel/octree/BaseCube.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation "BaseCube.h"
#endif
//
// Class: BaseCube
// Author: Geoffrey Cross, Oxford RRG
// Created: 11 May 99
// Modifications:
//   990511 Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include "BaseCube.h"

// Default ctor
BaseCube::BaseCube( const double x, const double y, const double z, const double s)
  : bases(s), basex(x), basey(y), basez(z)
{
}

