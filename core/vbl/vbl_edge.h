// This is core/vbl/vbl_edge.h
#ifndef vbl_edge_h_
#define vbl_edge_h_
//:
// \file
// \brief A class representing a graph edge with integer vertex ids
// \author J. Mundy
// \date February 15, 2013
// \verbatim
// \endverbatim
//-----------------------------------------------------------------------------
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class vbl_edge
{
 public:
  vbl_edge(): v0_(-1), v1_(-1), w_(0.0f) {}
  vbl_edge(int v0, int ve, float w): v0_(v0), v1_(ve) , w_(w){}
  void print(std::ostream& os) const { os << '(' << v0_ << ' ' << v1_ << ")\n"; }
  int v0_;
  int v1_;
  float w_;
};
bool operator<(const vbl_edge& e0, const vbl_edge& e1);
#endif // vbl_edge_h_
