// This is gel/vtol/tests/test_edge_2d.cxx
#include <testlib/testlib_test.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_zero_chain.h>

static void test_edge_2d()
{
  vcl_cout << "testing edge 2d\n";

  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(3.0,3.0);

  vtol_edge_2d_sptr e1= new vtol_edge_2d(v1,v2);
  vtol_edge_2d_sptr e1a = new vtol_edge_2d(e1);
  e1a->describe(vcl_cout,8);

  TEST("vtol_edge_2d equality", *e1, *e1a);
  TEST("vtol_edge_2d::cast_to_edge()", e1->cast_to_edge()==0, false);

  vtol_zero_chain_sptr zc2 = new vtol_zero_chain(v2,v3);
  zc2->describe(vcl_cout,8);

  vtol_edge_2d_sptr e2 = new vtol_edge_2d(zc2);
  TEST("vtol_edge_2d inequality", *e2==*e1, false);

  vtol_zero_chain_sptr zc3 = new vtol_zero_chain(v3,v4);

  zero_chain_list z_list;

  z_list.push_back(zc3);
  z_list.push_back(e1->zero_chain());

  vtol_edge_2d_sptr e3 = new vtol_edge_2d(z_list);
  TEST("vtol_edge_2d inequality", *e3==*e1, false);
  TEST("vtol_edge_2d inequality", *e3==*e2, false);

  vtol_edge_2d_sptr e4 = new vtol_edge_2d(3.0,3.0,4.0,4.0);
  vsol_spatial_object_2d_sptr so_clone = e4->clone();
  so_clone->describe(vcl_cout,8);
  vtol_topology_object_sptr to_clone = so_clone->cast_to_topology_object();
  to_clone->describe(vcl_cout,8);
  vtol_edge_2d_sptr e4_clone = to_clone->cast_to_edge()->cast_to_edge_2d();

  TEST("vtol_edge_2d::clone()", *e4, *e4_clone);
  TEST("vtol_edge_2d inequality", *e4==*e1, false);
  TEST("vtol_edge_2d inequality", *e4==*e2, false);
  TEST("vtol_edge_2d inequality", *e4==*e3, false);
  TEST("vtol_edge_2d::clone()", *e4, *(e4_clone->cast_to_topology_object()));
  TEST("vtol_edge_2d::cast_to_edge_2d()", e4->cast_to_edge_2d()==0, false);

  vtol_vertex_sptr e1v1 = e1->v1();
  vtol_vertex_sptr e1v2 = e1->v2();

  TEST("vtol_edge_2d equality", *e1v1, *v1);
  TEST("vtol_edge_2d equality", *e1v2, *v2);

  vtol_zero_chain_sptr e2zc = e2->zero_chain();
  TEST("vtol_edge_2d::zero_chain()", *e2zc, *zc2);

  e1->set_v1(v3->cast_to_vertex());
  e1->set_v2(v4->cast_to_vertex());

  TEST("vtol_edge_2d::v1()", *(e1->v1()), *v3);
  TEST("vtol_edge_2d::v2()", *(e1->v2()), *v4);

  vtol_vertex_sptr current = e1->v1();

  e1->replace_end_point(*current,*(v1));

  TEST("vtol_edge_2d::v1()", *(e1->v1()), *v1);
  TEST("vtol_edge_2d::cast_to_edge()", e1->cast_to_edge()==0, false);

  TEST("vtol_edge_2d::valid_inferior_type()",e1->valid_inferior_type(zc2),true);
  TEST("vtol_zero_chain::valid_superior_type()",zc2->valid_superior_type(e1),true);
  TEST("vtol_edge_2d::valid_inferior_type()",e1->valid_inferior_type(e1->cast_to_topology_object()),false);
}

TESTMAIN(test_edge_2d);
