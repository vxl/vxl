// This is gel/vtol/tests/test_zero_chain.cxx
#include <testlib/testlib_test.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_sptr.h>

static void test_zero_chain()
{
  vcl_cout << "testing zero chain\n";

  vtol_vertex_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_sptr v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_sptr v3 = new vtol_vertex_2d(2.0,2.0);

  vtol_zero_chain_sptr zc1 = new vtol_zero_chain(v1,v2);
  zc1->describe(vcl_cout,8);
  TEST("vtol_zero_chain equality", *zc1, *zc1);
  vtol_edge_sptr e1 = new vtol_edge_2d(v1, v2);
  TEST("edge's vtol_zero_chain equality", *zc1, *(e1->zero_chain()));

  vtol_zero_chain_sptr zc1_clone = zc1->clone()->cast_to_topology_object()->cast_to_zero_chain();
  TEST("vtol_zero_chain::clone()", *zc1, *zc1_clone);

  vertex_list v_list;
  v_list.push_back(v1);
  v_list.push_back(v2);
  vtol_zero_chain_sptr zc1a = new vtol_zero_chain(v_list);
  TEST("vtol_zero_chain equality", *zc1, *zc1a);

  v_list.clear();
  v_list.push_back(v2);
  v_list.push_back(v3);
  zc1a = new vtol_zero_chain(v_list);
  TEST("vtol_zero_chain inequality", *zc1 == *zc1a, false);

  TEST("vtol_zero_chain::valid_inferior_type()", zc1->valid_inferior_type(v1), true);
  TEST("vtol_zero_chain::valid_superior_type()", zc1->valid_superior_type(e1), true);
  TEST("vtol_vertex::valid_superior_type()", v1->valid_superior_type(zc1), true);
  TEST("vtol_vertex::valid_inferior_type()", v1->valid_inferior_type(zc1->cast_to_topology_object()), false);
  TEST("vtol_zero_chain::length()", zc1->length(), 2);
}

TESTMAIN(test_zero_chain);
