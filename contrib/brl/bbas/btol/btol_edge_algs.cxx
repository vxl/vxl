// This is brl/bbas/btol/btol_edge_algs.cxx
#include "btol_edge_algs.h"
//:
// \file
#include <vcl_algorithm.h> // vcl_find()
#include <vcl_cmath.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge_2d.h>
#include <vcl_iostream.h>

const double btol_edge_algs::tol = 1e-6;

//: Destructor
btol_edge_algs::~btol_edge_algs()
{
}

//-----------------------------------------------------------------------------
//: Splits e at v and returns the two edges e1, e2, which are incident at v.
// If v is not within btol_edge_algs::tol (user-settable) of a point on edge e
// then false is returned, and e is not split
//-----------------------------------------------------------------------------
bool btol_edge_algs::split_edge_2d(vtol_vertex_2d_sptr const& /*v*/,
                                   vtol_edge_2d_sptr const& /*e*/,
                                   vtol_edge_2d_sptr& /*e1*/, vtol_edge_2d_sptr& /*e2*/)
{
  vcl_cout << "tol = " << btol_edge_algs::tol << vcl_endl
           << "btol_edge_algs::split_edge_2d - not implemented\n";
  return true;
}

bool btol_edge_algs::unlink_all_inferiors_twoway(vtol_edge_2d_sptr const& e)
{
  if (!e->v1()||!e->v2())
    return false;
  vtol_vertex_sptr tv1 = e->v1()->cast_to_vertex();
  vtol_vertex_sptr tv2 = e->v2()->cast_to_vertex();

  vtol_edge_sptr toe = e->cast_to_edge();
  vcl_vector<vtol_topology_object_sptr>* infs = toe->inferiors();
  //this will be the zero_chain for the edge
  //Can have an edge with no vertices
  if (!infs->size())
    return true;
  if (infs->size()>1)
    {
      vcl_cout << " In btol_edge_algs::unlink_all_inferiors_twoway(..) -"
               << " inferiors inconsistent size\n";
      return false;
    }
  vtol_zero_chain_sptr inf_zero_chain = infs->front()->cast_to_zero_chain();
  if (!inf_zero_chain)
    {
      vcl_cout << " In btol_edge_algs::unlink_all_inferiors_twoway(..) -"
               << " null zero chain\n";
      return false;
    }
  inf_zero_chain->unlink_inferior(tv1);
  if (tv1!=tv2)
    inf_zero_chain->unlink_inferior(tv2);
  toe->unlink_inferior(inf_zero_chain);
  return true;
}

//-----------------------------------------------------------------------------
//: Replaces va by vb on edge e.
//-----------------------------------------------------------------------------
bool btol_edge_algs::subst_vertex_on_edge(vtol_vertex_sptr const& va,
                                          vtol_vertex_sptr const& vb,
                                          vtol_edge_sptr const& e)
{
  if (!va||!vb||!e)
    return false;
  vtol_vertex_sptr v1 = e->v1();
  vtol_vertex_sptr v2 = e->v2();
  if (!v1||!v2)
    return false;
  if (v1==va)
    {
      e->set_v1(vb);
      return true;
    }
  if (v2==va)
    {
      e->set_v2(vb);
      return true;
    }
  return false;
}

//-----------------------------------------------------------------------------
//: Computes the bounding box for a set of edges
//-----------------------------------------------------------------------------
vsol_box_2d btol_edge_algs::bounding_box(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  vsol_box_2d b;//default box

  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {
      vsol_curve_2d_sptr c = (*eit)->curve();
      if (!c)
        {
          vcl_cout << "In btol_edge_algs::bounding_box(..) - null curve\n";
          continue;
        }
      if (c->cast_to_digital_curve())
        b.grow_minmax_bounds(*c->cast_to_vdgl_digital_curve()->get_bounding_box());
      else
        vcl_cout << "In btol_edge_algs::bounding_box(..) -"
                 << " curve has unknown geometry\n";
    }
  return b;
}

//: a bit more convenient interface for finding edges than messing with iterators
void btol_edge_algs::edge_2d_erase(vcl_vector<vtol_edge_2d_sptr>& edges,
                                   vtol_edge_2d_sptr const& e)
{
  vcl_vector<vtol_edge_2d_sptr>::iterator eit =
    vcl_find(edges.begin(), edges.end(), e);
  if (eit != edges.end())
    edges.erase(eit);
  return;
}

//: find the vertex closest to the given position and return it
vtol_vertex_2d_sptr btol_edge_algs::closest_vertex(vtol_edge_2d_sptr const& e,
                                                   const double x,
                                                   const double y)
{
  double x1 = e->v1()->cast_to_vertex_2d()->x();
  double y1 = e->v1()->cast_to_vertex_2d()->y();
  double x2 = e->v2()->cast_to_vertex_2d()->x();
  double y2 = e->v2()->cast_to_vertex_2d()->y();
  double d1 = vcl_sqrt((x1-x)*(x1-x)+(y1-y)*(y1-y));
  double d2 = vcl_sqrt((x2-x)*(x2-x)+(y2-y)*(y2-y));
  if (d1<d2)
    return e->v1()->cast_to_vertex_2d();
  else
    return e->v2()->cast_to_vertex_2d();
}
