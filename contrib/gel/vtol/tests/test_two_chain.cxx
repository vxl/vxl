// This is gel/vtol/tests/test_two_chain.cxx
#include <testlib/testlib_test.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_two_chain_sptr.h>

static void test_two_chain()
{
  vcl_cout << "testing two chain\n";

  vertex_list v_list1;
  vtol_vertex_sptr v1 = new vtol_vertex_2d(0.0,0.0); v_list1.push_back(v1);
  vtol_vertex_sptr v2 = new vtol_vertex_2d(1.0,0.0); v_list1.push_back(v2);
  vtol_vertex_sptr v3 = new vtol_vertex_2d(1.0,1.0); v_list1.push_back(v3);
  vtol_vertex_sptr v4 = new vtol_vertex_2d(0.0,1.0); v_list1.push_back(v4);
  vtol_face_sptr f1 = new vtol_face_2d(v_list1);

  vertex_list v_list2;
  vtol_vertex_sptr v5 = new vtol_vertex_2d(1.0,2.0); v_list2.push_back(v5);
  vtol_vertex_sptr v6 = new vtol_vertex_2d(1.0,3.0); v_list2.push_back(v6);
  vtol_vertex_sptr v7 = new vtol_vertex_2d(2.0,3.0); v_list2.push_back(v7);
  vtol_vertex_sptr v8 = new vtol_vertex_2d(2.0,2.0); v_list2.push_back(v8);
  vtol_face_sptr f2 = new vtol_face_2d(v_list2);

  face_list f_list1;
  f_list1.push_back(f1);
  f_list1.push_back(f2);

  vtol_two_chain_sptr tc1 = new vtol_two_chain(f_list1);
  tc1->describe(vcl_cout,8);

  vcl_vector<signed char> dirs;
  dirs.push_back(-1);
  dirs.push_back(1);

  vtol_two_chain_sptr tc2 = new vtol_two_chain(f_list1,dirs);
  tc2->describe(vcl_cout,8);
  TEST("vtol_two_chain inequality (directions)", *tc1 == *tc2, false);

  vtol_two_chain_sptr tc1_copy = new vtol_two_chain(tc1);
  TEST("vtol_two_chain deep copy (pseudo copy constructor)", *tc1, *tc1_copy);

  vsol_spatial_object_2d_sptr tc2_clone = tc2->clone();
  tc2_clone->describe(vcl_cout,8);

  TEST("vtol_two_chain::clone()", *tc2, *tc2_clone);

  TEST("vtol_two_chain::direction()", tc1->direction(*f1), 1);
  TEST("vtol_two_chain::direction()", tc1->direction(*f2), 1);
  TEST("vtol_two_chain::direction()", tc2->direction(*f1), -1);
  TEST("vtol_two_chain::direction()", tc2->direction(*f2), 1);

  TEST("vtol_two_chain::face()", tc1->face(0), f1);

  vertex_list v_list3;
  vtol_vertex_sptr v9 = new vtol_vertex_2d(1.0,0.0); v_list3.push_back(v9);
  vtol_vertex_sptr v10 = new vtol_vertex_2d(2.0,1.0); v_list3.push_back(v10);
  vtol_vertex_sptr v11 = new vtol_vertex_2d(3.0,2.0); v_list3.push_back(v11);
  vtol_vertex_sptr v12 = new vtol_vertex_2d(4.0,3.0); v_list3.push_back(v12);
  vtol_face_sptr f3 = new vtol_face_2d(v_list3);
  tc1->add_face(f3,1);
  TEST("vtol_two_chain::add_face()", tc1->face(2), f3);

  tc1->remove_face(f2);
  TEST("vtol_two_chain::remove_face()", tc1->face(1), f3);

  TEST("vtol_two_chain::cast_to_two_chain()", tc1->cast_to_two_chain()==0, false);
  TEST("vtol_two_chain::valid_inferior_type()", tc1->valid_inferior_type(f1), true);
  TEST("vtol_two_chain::valid_superior_type()", f1->valid_superior_type(tc1), true);
  TEST("vtol_two_chain::valid_chain_type()", tc1->valid_chain_type(tc1), true);
  TEST("vtol_two_chain::num_faces()", tc1->num_faces(), 2);

  tc1->link_chain_inferior(tc2);
  vtol_two_chain_sptr new_copy = new vtol_two_chain(tc1);
  TEST("vtol_two_chain deep copy (pseudo copy constructor)", *new_copy, *tc1);

  vertex_list *v_list = tc1->outside_boundary_vertices();
  TEST("vtol_two_chain::outside_boundary_vertices()", v_list->size(), 8);
  delete v_list;

  zero_chain_list *z_list = tc1->outside_boundary_zero_chains();
  TEST("vtol_two_chain::outside_boundary_zero_chains()", z_list->size(), 16);
  for (unsigned int i=0; i<z_list->size(); ++i)
    (*z_list)[i]->describe(vcl_cout,8);
  delete z_list;

  edge_list *ed_list = tc1->outside_boundary_edges();
  TEST("vtol_two_chain::outside_boundary_edges()", ed_list->size(), 8);
  delete ed_list;

  one_chain_list *o_list = tc1->outside_boundary_one_chains();
  TEST("vtol_two_chain::outside_boundary_one_chains()", o_list->size(), 2);
  delete o_list;

  face_list *f_list = tc1->outside_boundary_faces();
  TEST("vtol_two_chain::outside_boundary_faces()", f_list->size(), 2);
  delete f_list;
}

TESTMAIN(test_two_chain);
