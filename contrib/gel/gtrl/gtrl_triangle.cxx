/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gtrl_triangle"
#endif
#include "gtrl_triangle.h"

gtrl_triangle::gtrl_triangle( gtrl_vertex_ref p1, gtrl_vertex_ref p2, gtrl_vertex_ref p3)
  : p1_(p1), p2_(p2), p3_(p3)
{
}
