/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "gtrl_triangle.h"

gtrl_triangle::gtrl_triangle( gtrl_vertex_ref p1, gtrl_vertex_ref p2, gtrl_vertex_ref p3)
  : p1_(p1), p2_(p2), p3_(p3),
    midpoint_( new gtrl_vertex( (p1->x()+p2->x()+p3->x())/3, 
				(p1->y()+p2->y()+p3->y())/3))
{
}
