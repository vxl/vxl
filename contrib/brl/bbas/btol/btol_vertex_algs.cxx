#include <btol/btol_vertex_algs.h>
#include <btol/btol_edge_algs.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge_2d.h>
#include <vcl_iostream.h>


//: Destructor
btol_vertex_algs::~btol_vertex_algs()
{
}

//:
//-----------------------------------------------------------------------------
// Replaces va by vb on all the edges connected to va.
// The result is that vb has the union of the sets of 
// edges incident on va and vb as superiors.
//-----------------------------------------------------------------------------
bool btol_vertex_algs::merge_superiors(vtol_vertex_sptr& va,
                                       vtol_vertex_sptr& vb)
{
  if(!va||!vb)
    return false;
  vcl_vector<vtol_edge_sptr>* edges = va->edges();
  for(vcl_vector<vtol_edge_sptr>::iterator eit = edges->begin();
      eit != edges->end(); eit++)
    btol_edge_algs::subst_vertex_on_edge(va, vb, *eit);
  delete edges;
  return true;
}

