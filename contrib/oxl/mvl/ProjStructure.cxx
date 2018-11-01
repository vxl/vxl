// This is oxl/mvl/ProjStructure.cxx

#include "ProjStructure.h"

#include <mvl/HomgPoint2D.h>

static HomgPoint2D x(0,0,0);

HomgPoint2D& ProjStructure::get_point_2d(int /*view*/, int /*c*/)
{
  return x;
}

int ProjStructure::get_point_match(int /*view*/, int /*c*/)
{
  return -1;
}
