// This is gel/gtrl/gtrl_polygon.h
#ifndef gtrl_polygon_h_
#define gtrl_polygon_h_
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <gtrl/gtrl_vertex.h>

class gtrl_polygon
{
 public:
  gtrl_polygon( const std::vector<gtrl_vertex_sptr>& ps);

  int size() const { return ps_.size(); }
  gtrl_vertex_sptr operator[]( const int i) const { return ps_[i]; }

  // computations
  bool inside( const gtrl_vertex_sptr& point) const;

 protected:
  std::vector<gtrl_vertex_sptr> ps_;
};

#endif // gtrl_polygon_h_
