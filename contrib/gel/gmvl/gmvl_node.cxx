/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "gmvl_node.h"


gmvl_node::gmvl_node()
  : type_( "gmvl_node"), ref_( -1)
{
}

gmvl_node::~gmvl_node()
{
}

vcl_ostream &operator<<( vcl_ostream &os, gmvl_node &node)
{
  return ( os << "<Node " << &node << " ref=" << node.ref_ << " type=\"" << node.type_ << "\">");
}
