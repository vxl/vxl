/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_tag_node"
#endif
#include "gmvl_tag_node.h"

gmvl_tag_node::gmvl_tag_node( const vcl_string name)
  : name_( name)
{
  type_= "gmvl_tag_node";
}

gmvl_tag_node::~gmvl_tag_node()
{
}

