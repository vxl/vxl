#include <cmath>
#include <iostream>
#include <algorithm>
#include "btol_vertex_algs.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "btol_edge_algs.h"
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_sptr.h>

//: Destructor
btol_vertex_algs::~btol_vertex_algs()
= default;

//:
//-----------------------------------------------------------------------------
// Replaces va by vb on all the edges connected to va.
// The result is that vb has the union of the sets of
// edges incident on va and vb as superiors.
//-----------------------------------------------------------------------------
bool btol_vertex_algs::merge_superiors(vtol_vertex_sptr& va,
                                       vtol_vertex_sptr& vb)
{
  if (!va||!vb)
    return false;
  std::vector<vtol_edge_sptr> edges; va->edges(edges);
  for (auto & edge : edges)
    btol_edge_algs::subst_vertex_on_edge(va, vb, edge);
  return true;
}

#if 0 // unused static function
static void vertex_erase(std::vector<vtol_vertex_sptr>& verts,
                         vtol_vertex_sptr& v)
{
  std::vector<vtol_vertex_sptr>::iterator vit =
    std::find(verts.begin(), verts.end(), v);
  if (vit == verts.end())
    return;
  verts.erase(vit);
  return;
}
#endif // 0

vtol_vertex_2d_sptr btol_vertex_algs::
transform(vtol_vertex_2d_sptr const& v,
          vnl_matrix_fixed<double,3,3> const& T)
{
  vnl_vector_fixed<double,3> P(v->x(), v->y(), 1.0);
  vnl_vector_fixed<double,3> p = T*P;
  if (std::fabs(p[2])<1e-06)
    return nullptr;
  else
    return new vtol_vertex_2d(p[0]/p[2], p[1]/p[2]);
}
