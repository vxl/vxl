// This is gel/vtol/tests/test_one_chain.cxx
#include <testlib/testlib_test.h>
#include <vsol/vsol_point_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>

static void test_one_chain()
{
  vcl_cout << "testing one_chain\n";

  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(3.0,3.0);

  edge_list e_list;
  vtol_edge_sptr e12 = new vtol_edge_2d(v1,v2); e_list.push_back(e12);
  vtol_edge_sptr e23 = new vtol_edge_2d(v2,v3); e_list.push_back(e23);
  vtol_edge_sptr e34 = new vtol_edge_2d(v3,v4); e_list.push_back(e34);
  vtol_edge_sptr e41 = new vtol_edge_2d(v4,v1); e_list.push_back(e41);
  vtol_one_chain_sptr oc1 = new vtol_one_chain(e_list);
  oc1->describe(vcl_cout,8);

  vcl_vector<signed char> dirs;
  dirs.push_back(1);
  dirs.push_back(1);
  dirs.push_back(1);
  dirs.push_back(1);

  vtol_one_chain_sptr oc2 = new vtol_one_chain(e_list,dirs);

  TEST("vtol_one_chain equality", *oc1, *oc2);

  vtol_one_chain_sptr oc3 = new vtol_one_chain(oc2);

  TEST("vtol_one_chain deep copy (pseudo copy constructor)", *oc2, *oc3);

  vsol_spatial_object_2d_sptr so_oc_clone = oc3->clone();
  so_oc_clone->describe(vcl_cout,8);
  vtol_one_chain_sptr oc3_clone = so_oc_clone->cast_to_topology_object()->cast_to_one_chain();
  TEST("vtol_one_chain::clone()", *oc3_clone, *oc3);

  TEST("vtol_one_chain::direction()", oc2->direction(*e12), 1);

  TEST("vtol_one_chain::cast_to_one_chain()", oc2->cast_to_one_chain()==0, false);
  TEST("vtol_one_chain::valid_inferior_type()", oc2->valid_inferior_type(e12), true);
  TEST("vtol_edge::valid_superior_type()", e12->valid_superior_type(oc2), true);
  TEST("vtol_one_chain::valid_inferior_type()", oc2->valid_inferior_type(v1->cast_to_topology_object()), false);
  TEST("vtol_edge::valid_inferior_type()", e12->valid_inferior_type(oc2->cast_to_topology_object()), false);

  vertex_list *v_list = oc1->outside_boundary_vertices();
  TEST("vtol_one_chain::outside_boundary_vertices()", v_list->size(), 4);
  delete v_list;

  zero_chain_list *z_list = oc1->outside_boundary_zero_chains();
  oc1->describe(vcl_cout,8);

  vcl_cout<<"outside_boundary_zero_chains()->size() = "<< z_list->size()<<'\n';
  for (unsigned int i=0; i<z_list->size(); ++i)
    (*z_list)[i]->describe(vcl_cout,8);

  TEST("vtol_one_chain::outside_boundary_zero_chains()", z_list->size(), 4);
  delete z_list;

  edge_list *ed_list = oc1->outside_boundary_edges();
  TEST("vtol_one_chain::outside_boundary_edges()", ed_list->size(), 4);
  delete ed_list;

  one_chain_list *o_list = oc1->outside_boundary_one_chains();
  TEST("vtol_one_chain::outside_boundary_one_chains()", o_list->size(), 1);
  delete o_list;

  // add some holes to oc1;

  vtol_vertex_sptr vh1 = new vtol_vertex_2d(0.1,0.1);
  vtol_vertex_sptr vh2 = new vtol_vertex_2d(1.1,1.1);
  vtol_vertex_sptr vh3 = new vtol_vertex_2d(2.1,2.1);
  vtol_vertex_sptr vh4 = new vtol_vertex_2d(3.1,3.1);

  e_list.clear();
  vtol_edge_sptr eh12 = new vtol_edge_2d(vh1,vh2); e_list.push_back(eh12);
  vtol_edge_sptr eh23 = new vtol_edge_2d(vh2,vh3); e_list.push_back(eh23);
  vtol_edge_sptr eh34 = new vtol_edge_2d(vh3,vh4); e_list.push_back(eh34);
  vtol_edge_sptr eh41 = new vtol_edge_2d(vh4,vh1); e_list.push_back(eh41);
  vtol_one_chain_sptr och1 = new vtol_one_chain(e_list);
  oc1->link_chain_inferior(och1);

  one_chain_list *ic_list = oc1->inferior_one_chains();
  TEST("vtol_one_chain::inferior_one_chains()", ic_list->size(), 1);
  delete ic_list;

  one_chain_list *sc_list = och1->superior_one_chains();
  TEST("vtol_one_chain::superior_one_chains()", *(*(sc_list->begin())), *oc1);
  delete sc_list;

  TEST("vtol_one_chain::num_edges()", oc1->num_edges(), 4);
  int dir = oc1->dir(0);
  oc1->reverse_directions();
  TEST("vtol_one_chain::reverse_directions()", dir == oc1->dir(0), false);
  TEST("vtol_one_chain::edge()", *(oc1->edge(1)), *e34);

  oc1->reverse_directions();

  vtol_edge_2d_sptr n_e = new vtol_edge_2d(5,5,1,1);

  oc1->add_edge(n_e,true);
  TEST("vtol_one_chain::add_edge()", oc1->num_edges(), 5);
  TEST("vtol_one_chain::add_edge()", *(oc1->edge(4)), *n_e);

  oc1->remove_edge(n_e,true);
  TEST("vtol_one_chain::remove_edge()", oc1->num_edges(), 4);

  vsol_spatial_object_2d_sptr oc1_clone = oc1->clone();

  TEST("vtol_one_chain::clone()", *oc1, *oc1_clone);
  TEST("vtol_one_chain inequality", *oc1 == *och1, false);
  //==========================================================================
  // JLM add a test for the bounding box method use a digital_curve and several
  // line segments as the geometry - note that this test covers edge and vertex
  // bounding box methods as well
  //==========================================================================
  // construct a digital_curve
  vsol_point_2d_sptr p0  = new vsol_point_2d(1, 1);
  vsol_point_2d_sptr p1  = new vsol_point_2d(5, 5);
  vsol_curve_2d_sptr dc  = new vdgl_digital_curve(p0, p1);
  vtol_vertex_2d_sptr vd0 = new vtol_vertex_2d(*p0);
  vtol_vertex_2d_sptr vd1 = new vtol_vertex_2d(*p1);
  vtol_edge_sptr e0   = new vtol_edge_2d(vd0, vd1, dc);
  //complete the triangle
  vtol_vertex_2d_sptr vd2 = new vtol_vertex_2d(0, 4);
  vtol_edge_sptr e1   = new vtol_edge_2d(vd1, vd2);
  vtol_edge_sptr e2   = new vtol_edge_2d(vd2, vd0);
  edge_list edges;
  edges.push_back(e0);   edges.push_back(e1);   edges.push_back(e2);
  vtol_one_chain_sptr onch = new vtol_one_chain(edges, true);
  vcl_cout << "one chain bounds (" << onch->get_min_x() << ' ' << onch->get_min_y()
           << '|' << onch->get_max_x() << ' ' << onch->get_max_y() << ")\n";
  TEST("vtol_one_chain::get_max_x()", onch->get_max_x()==5&&onch->get_max_y()==5, true);
}

TESTMAIN(test_one_chain);
