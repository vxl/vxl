#include <vcl_fstream.h>

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  vcl_cout << "testing one  chain" << vcl_endl;

  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(3.0,3.0);

  vtol_edge_sptr e12 = new vtol_edge_2d(*v1,*v2);
  vtol_edge_sptr e23 = new vtol_edge_2d(*v2,*v3);
  vtol_edge_sptr e34 = new vtol_edge_2d(*v3,*v4);
  vtol_edge_sptr e41 = new vtol_edge_2d(*v4,*v1);

  edge_list e_list;

  e_list.push_back(e12);
  e_list.push_back(e23);
  e_list.push_back(e34);
  e_list.push_back(e41);

  vtol_one_chain_sptr oc1 = new vtol_one_chain(e_list);

  vcl_vector<signed char> dirs;
  dirs.push_back(1);
  dirs.push_back(1);
  dirs.push_back(1);
  dirs.push_back(1);

  vtol_one_chain_sptr oc2 = new vtol_one_chain(e_list,dirs);

  vtol_one_chain_sptr oc3 = new vtol_one_chain(*oc2);

  Assert(*oc2==*oc3);

  vsol_spatial_object_3d_sptr so_oc_clone = oc3->clone();
  vtol_one_chain_sptr oc3_clone = so_oc_clone->cast_to_topology_object()->cast_to_one_chain();

  Assert(*oc2==*oc3);

  Assert(oc2->direction(*e12)==1);

  Assert(oc2->cast_to_one_chain()!=0);
  Assert(oc2->valid_inferior_type(*(e12)));
  Assert(!(oc2->valid_inferior_type(*v1)));

  vertex_list *v_list = oc1->outside_boundary_vertices();
  Assert(v_list->size()==4);

  zero_chain_list *z_list = oc1->outside_boundary_zero_chains();
  Assert(z_list->size()==4);

  edge_list  *ed_list = oc1->outside_boundary_edges();
  Assert(ed_list->size()==4);

  one_chain_list *o_list =  oc1->outside_boundary_one_chains();
  Assert(o_list->size()==1);

  delete v_list;
  delete z_list;
  delete ed_list;
  delete o_list;

  // add some holes to oc1;

  vtol_vertex_2d_sptr vh1 = new vtol_vertex_2d(0.1,0.1);
  vtol_vertex_2d_sptr vh2 = new vtol_vertex_2d(1.1,1.1);
  vtol_vertex_2d_sptr vh3 = new vtol_vertex_2d(2.1,2.1);
  vtol_vertex_2d_sptr vh4 = new vtol_vertex_2d(3.1,3.1);

  e_list.clear();

  vtol_edge_sptr eh12 = new vtol_edge_2d(*vh1,*vh2);
  vtol_edge_sptr eh23 = new vtol_edge_2d(*vh2,*vh3);
  vtol_edge_sptr eh34 = new vtol_edge_2d(*vh3,*vh4);
  vtol_edge_sptr eh41 = new vtol_edge_2d(*vh4,*vh1);

  e_list.push_back(eh12);
  e_list.push_back(eh23);
  e_list.push_back(eh34);
  e_list.push_back(eh41);

  vtol_one_chain_sptr och1 = new vtol_one_chain(e_list);

  oc1->link_chain_inferior(*och1);

  one_chain_list *ic_list = oc1->inferior_one_chains();
  Assert(ic_list->size()==1);

  one_chain_list *sc_list = och1->superior_one_chains();

  Assert(*(*(sc_list->begin()))==*oc1);

  int dir = oc1->dir(0);

  oc1->reverse_directions();

  Assert(oc1->edge(1)==e34.ptr());
  Assert(oc1->num_edges()==4);
  Assert(dir != oc1->dir(0));

  oc1->reverse_directions();

  vtol_edge_2d_sptr n_e = new vtol_edge_2d(5,5,1,1);

  oc1->add_edge(*n_e,true);

  Assert(oc1->num_edges()==5);
  Assert(oc1->edge(4)==n_e.ptr());

  oc1->remove_edge(*n_e,true);

  Assert(oc1->num_edges()==4);

  vsol_spatial_object_3d_sptr oc1_clone = oc1->clone();

  Assert(*oc1 == *oc1_clone);
  Assert(!(*oc1 == *och1));
  Assert(oc1->topology_type()==vtol_topology_object::ONECHAIN);

  vcl_cout << "Finished testing one chain" << vcl_endl;
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
