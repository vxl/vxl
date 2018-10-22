// This is gel/gmvl/gmvl_connection.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include "gmvl_connection.h"
#include <vcl_compiler.h>

gmvl_connection::gmvl_connection( const gmvl_node_sptr node1, const gmvl_node_sptr node2)
  : n1_( node1), n2_( node2)
{
}

gmvl_connection::~gmvl_connection() = default;

// input / output

std::ostream &operator<<( std::ostream &os, const gmvl_connection &c)
{
  return os << "<Connection n1=" << c.n1_.ptr() << " n2=" << c.n2_.ptr() << '>';
}
