// This is gel/gmvl/gmvl_corner_node.h
#ifndef gmvl_corner_node_h_
#define gmvl_corner_node_h_
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <gmvl/gmvl_node.h>

class gmvl_corner_node : public gmvl_node
{
 public:
  gmvl_corner_node(double x, double y) : x_(x),y_(y) {type_="gmvl_corner_node";}
  ~gmvl_corner_node() {}

  double x() const { return x_; }
  double y() const { return y_; }

 protected:
  double x_;
  double y_;
};

#endif // gmvl_corner_node_h_
