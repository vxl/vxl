#ifndef gmvl_corner_node_h_
#define gmvl_corner_node_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

#include <gmvl/gmvl_node.h>

class gmvl_corner_node : public gmvl_node
{
public:

  gmvl_corner_node( double x, double y);
  ~gmvl_corner_node();

  double x() const { return x_; }
  double y() const { return y_; }

protected:

  double x_;
  double y_;
};

#endif // gmvl_corner_node_h_
