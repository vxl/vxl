/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "gmvl_corner_node.h"

gmvl_corner_node::gmvl_corner_node( double x, double y)
  : x_(x), y_(y)
{
  type_= "gmvl_corner_node";
}

gmvl_corner_node::~gmvl_corner_node()
{
}
