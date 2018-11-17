// This is gel/gmvl/gmvl_connection.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include "gmvl_connection.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

gmvl_connection::gmvl_connection( const gmvl_node_sptr& node1, const gmvl_node_sptr& node2)
  : n1_( node1), n2_( node2)
{
}

gmvl_connection::~gmvl_connection() = default;

// input / output

std::ostream &operator<<( std::ostream &os, const gmvl_connection &c)
{
  return os << "<Connection n1=" << c.n1_.ptr() << " n2=" << c.n2_.ptr() << '>';
}
