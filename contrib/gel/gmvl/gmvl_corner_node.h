#ifndef gmvl_corner_node_h_
#define gmvl_corner_node_h_
#ifdef __GNUC__
#pragma interface "gmvl_corner_node"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_string.h>
#include <gmvl/gmvl_node.h>

class gmvl_corner_node : public gmvl_node
{
public:

  gmvl_corner_node( const vcl_string type, double x, double y);
  ~gmvl_corner_node();

protected:

  double x_;
  double y_;

};

#endif
