// This is gel/gtrl/gtrl_polygon.h
#ifndef gtrl_polygon_h_
#define gtrl_polygon_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#  pragma interface
#endif
// :
// \file
// \author crossge@crd.ge.com

#include <vcl_vector.h>
#include <gtrl/gtrl_vertex.h>

class gtrl_polygon
{
public:
  gtrl_polygon( const vcl_vector<gtrl_vertex_sptr> ps);

  int size() const { return ps_.size(); }
  gtrl_vertex_sptr operator[]( const int i) const { return ps_[i]; }

  // computations
  bool inside( const gtrl_vertex_sptr point) const;

protected:
  vcl_vector<gtrl_vertex_sptr> ps_;
};

#endif // gtrl_polygon_h_
