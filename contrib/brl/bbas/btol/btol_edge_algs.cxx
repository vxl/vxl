//:
// \file
#include <vcl_algorithm.h> // vcl_find()
#include <vdgl/vdgl_digital_curve.h>
#include <btol/btol_edge_algs.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge_2d.h>
#include <vcl_iostream.h>

const double btol_edge_algs::tol = 1e-6;

//: Destructor
btol_edge_algs::~btol_edge_algs()
{
}

//:
//-----------------------------------------------------------------------------
// Splits e at v and returns the two edges e1, e2, which are incident at v.
// If v is not within etol of a point on edge e then false is returned, and
// e is not split
//-----------------------------------------------------------------------------
bool btol_edge_algs::split_edge_2d(vtol_vertex_2d_sptr& v, vtol_edge_2d_sptr& e,
                                   vtol_edge_2d_sptr& e1, vtol_edge_2d_sptr& e2)
{
  vcl_cout << "tol " << btol_edge_algs::tol << vcl_endl;
  vcl_cout << "btol_edge_algs::split_edge_2d - not implemented\n";
 return true;
}

bool btol_edge_algs::unlink_all_inferiors_twoway(vtol_edge_2d_sptr& e)
{
  if (!e->v1()||!e->v2())
    return false;
  vtol_topology_object_sptr tv1 = e->v1()->cast_to_topology_object();
  vtol_topology_object_sptr tv2 = e->v2()->cast_to_topology_object();

  vtol_topology_object_sptr toe = e->cast_to_topology_object();
  vcl_vector<vtol_topology_object_sptr>* infs = toe->inferiors();
  //this will be the zero_chain for the edge
  if (infs->size()!=1)
    {
      vcl_cout << " In btol_edge_algs::unlink_all_inferiors_twoway(..) "
               << " inferiors inconsistent size\n";
      return false;
    }
  vtol_topology_object_sptr inf_two_chain = (*infs)[0];
  if (!inf_two_chain)
    {
      vcl_cout << " In btol_edge_algs::unlink_all_inferiors_twoway(..) "
               << " null two chain\n";
      return false;
    }
  toe->unlink_inferior(*inf_two_chain);
  inf_two_chain->unlink_inferior(*tv1);
  if(!(tv1==tv2))
	inf_two_chain->unlink_inferior(*tv2);
  return true;
}
//:
//-----------------------------------------------------------------------------
// Replaces va by vb on edge e.
//-----------------------------------------------------------------------------
bool btol_edge_algs::subst_vertex_on_edge(vtol_vertex_sptr& va,
                                          vtol_vertex_sptr& vb,
                                          vtol_edge_sptr& e)
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
//:
//-----------------------------------------------------------------------------
// Computes the bounding box for a set of edges
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
        b.grow_minmax_bounds(*c->cast_to_digital_curve()->get_bounding_box());
      else
        vcl_cout << "In btol_edge_algs::bounding_box(..) - curve has"
                 << " unknown geometry\n";
    }
  return b;
}
//: a bit more convenient interface for finding edges than messing with iterators
void btol_edge_algs::edge_2d_erase(vcl_vector<vtol_edge_2d_sptr>& edges,
                                   vtol_edge_2d_sptr& e)
{
  vcl_vector<vtol_edge_2d_sptr>::iterator eit =
    vcl_find(edges.begin(), edges.end(), e);
  if (eit == edges.end())
    return;
  edges.erase(eit);
  return;
}
