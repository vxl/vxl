#ifndef gtrl_polygon_h_
#define gtrl_polygon_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

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


#endif
