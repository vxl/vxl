#ifndef gtrl_polygon_h_
#define gtrl_polygon_h_
#ifdef __GNUC__
#pragma interface "gtrl_polygon"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_vector.h>
#include <gtrl/gtrl_vertex.h>

class gtrl_polygon
{
public:
  gtrl_polygon( const vcl_vector<gtrl_vertex_ref> ps);
 
  int size() const { return ps_.size(); }
  gtrl_vertex_ref operator[]( const int i) const { return ps_[i]; }
 
  // computations
  bool inside( const gtrl_vertex_ref point) const;

protected:
  vcl_vector<gtrl_vertex_ref> ps_;
};


#endif
