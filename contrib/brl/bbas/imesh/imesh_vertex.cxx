// This is brl/bbas/imesh/imesh_vertex.cxx

//:
// \file


#include "imesh_vertex.h"


//: compute the vector normal to the plane defined by 3 vertices
vgl_vector_3d<double> imesh_tri_normal(const imesh_vertex<3>& a,
                                       const imesh_vertex<3>& b,
                                       const imesh_vertex<3>& c)
{
  vgl_vector_3d<double> ac(c[0]-a[0],c[1]-a[1],c[2]-a[2]);
  vgl_vector_3d<double> ab(b[0]-a[0],b[1]-a[1],b[2]-a[2]);
  return cross_product(ab,ac);
}
