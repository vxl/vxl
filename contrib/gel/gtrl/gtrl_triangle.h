#ifndef gtrl_triangle_h_
#define gtrl_triangle_h_
#ifdef __GNUC__
#pragma interface "gtrl_triangle"
#endif
/*
  crossge@crd.ge.com
*/


#include "gtrl_vertex.h"

class gtrl_triangle
{
public:
  gtrl_triangle( gtrl_vertex_ref p1, gtrl_vertex_ref p2, gtrl_vertex_ref p3);

protected:
  gtrl_vertex_ref p1_;
  gtrl_vertex_ref p2_;
  gtrl_vertex_ref p3_;
};


#endif
