//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "ProjStructure.h"
#endif
//
// Class: ProjStructure
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 17 Mar 00
// Modifications:
//   000317 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "ProjStructure.h"

#include <mvl/HomgPoint2D.h>

static HomgPoint2D x(0,0,0);

HomgPoint2D& ProjStructure::get_point_2d(int view, int c)
{
  return x;
}

int ProjStructure::get_point_match(int view, int c)
{
  return -1;
}
