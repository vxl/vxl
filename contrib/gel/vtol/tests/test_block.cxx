// This is gel/vtol/tests/test_block.cxx
#include <testlib/testlib_test.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_two_chain_sptr.h>
#include <vtol/vtol_block.h>
#include <vtol/vtol_block_sptr.h>

static void test_block()
{
  vcl_cout << "testing block\n";

  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(3.0,3.0);

  vertex_list v_list1;

  v_list1.push_back(v1->cast_to_vertex());
  v_list1.push_back(v2->cast_to_vertex());
  v_list1.push_back(v3->cast_to_vertex());
  v_list1.push_back(v4->cast_to_vertex());

  vtol_face_2d_sptr f1 = new vtol_face_2d(v_list1);

  vtol_vertex_2d_sptr v5 = new vtol_vertex_2d(1.0,0.0);
  vtol_vertex_2d_sptr v6 = new vtol_vertex_2d(2.0,1.0);
  vtol_vertex_2d_sptr v7 = new vtol_vertex_2d(3.0,2.0);
  vtol_vertex_2d_sptr v8 = new vtol_vertex_2d(4.0,3.0);

  vertex_list v_list2;

  v_list2.push_back(v5->cast_to_vertex());
  v_list2.push_back(v6->cast_to_vertex());
  v_list2.push_back(v7->cast_to_vertex());
  v_list2.push_back(v8->cast_to_vertex());

  vtol_face_2d_sptr f2 = new vtol_face_2d(v_list2);

  face_list f_list1;

  f_list1.push_back(f1->cast_to_face());
  f_list1.push_back(f2->cast_to_face());

  vtol_two_chain_sptr tc1 = new vtol_two_chain(f_list1);
  vtol_two_chain_sptr tc1_copy = new vtol_two_chain(f_list1);
  tc1->describe(vcl_cout,8);

  TEST("vtol_two_chain equality", *tc1, *tc1_copy);
  vtol_block_sptr b1 = new vtol_block(*tc1);
  b1->describe(vcl_cout,8);

  vcl_vector<signed char> dirs;

  dirs.push_back(1);
  dirs.push_back(1);

  vtol_two_chain_sptr tc2 = new vtol_two_chain(f_list1,dirs);

  vtol_block_sptr b3 = new vtol_block(f_list1);
  TEST("vtol_block equality", *b3, *b1);
  b3->describe(vcl_cout,8);

  TEST("vtol_block::get_boundary_cycle()", b1->get_boundary_cycle(), tc1.ptr());

  TEST("vtol_block::get_boundary_cycle()", *(b1->get_boundary_cycle()), *(b3->get_boundary_cycle()));
  TEST("vtol_block::get_boundary_cycle()", b1->get_boundary_cycle()==0, false);

  vtol_block_sptr b1_copy = new vtol_block(*b1);
  TEST("vtol_block copy constructor", *b1, *b1_copy);
  b1_copy->describe(vcl_cout,8);

  TEST("vtol_block::get_boundary_cycle()", b1->get_boundary_cycle()==0, false);
  TEST("vtol_block::get_boundary_cycle()", b1->get_boundary_cycle()==0, false);
  TEST("vtol_block::get_boundary_cycle()", b1_copy->get_boundary_cycle()==0, false);
  TEST("vtol_block::get_boundary_cycle()", *(b1->get_boundary_cycle()), *(b1_copy->get_boundary_cycle()));

  two_chain_list tc_list;
  tc_list.push_back(tc2);
  tc_list.push_back(tc1);

  vtol_block_sptr b2 = new vtol_block(tc_list);

  TEST("vtol_block inequality", *b2 == *b1, false);
  TEST("vtol_block::get_boundary_cycle()", *(b2->get_boundary_cycle()), *tc2);

  vertex_list *verts = b2->vertices();
  TEST("vtol_block::vertices()", verts->size(), 8);
  delete verts;

  vtol_block_sptr b2_copy = new vtol_block(*b2);
  TEST("vtol_block copy constructor", *b2, *b2_copy);

  vsol_spatial_object_2d_sptr b2_clone = b2->clone();
  TEST("vtol_block::clone()", *b2, *b2_clone);

  TEST("vtol_block::cast_to_block()", b2->cast_to_block()==0, false);
  TEST("vtol_block::valid_inferior_type()",b1->valid_inferior_type(*tc1),true);
  TEST("vtol_block::valid_superior_type()",b1->valid_superior_type(*b1),false);
  TEST("vtol_two_chain::valid_superior_type()",tc1->valid_superior_type(*b1),true);
}

TESTLIB_DEFINE_MAIN(test_block);
