/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_corner_node"
#endif
#include "gmvl_corner_node.h"

gmvl_corner_node::gmvl_corner_node( const vcl_string name, double x, double y)
  : gmvl_node( name),
    x_(x), y_(y)
{
  type_= "gmvl_corner_node";
}

gmvl_corner_node::~gmvl_corner_node()
{
}
