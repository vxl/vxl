#include <vcl_fstream.h>

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_two_chain_sptr.h>
#include <vtol/vtol_block.h>
#include <vtol/vtol_block_sptr.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

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

  Assert(*tc1 == *tc1_copy);
  vtol_block_sptr b1 = new vtol_block(*tc1);

  vcl_vector<signed char> dirs;

  dirs.push_back(1);
  dirs.push_back(1);

  vtol_two_chain_sptr tc2 = new vtol_two_chain(f_list1,dirs);

  vtol_block_sptr b3 = new vtol_block(f_list1);

  Assert(b1->get_boundary_cycle() == tc1.ptr());

  Assert(*(b1->get_boundary_cycle()) == *(b3->get_boundary_cycle()));
  Assert(b1->get_boundary_cycle()!=0);
  Assert(*b3 == *b1);

  vtol_block_sptr b1_copy = new vtol_block(*b1);

  Assert(b1->get_boundary_cycle()!=0);
  Assert(b1->get_boundary_cycle()!=0);
  Assert(b1_copy->get_boundary_cycle()!=0);
  Assert(*(b1->get_boundary_cycle()) == *(b1_copy->get_boundary_cycle()));
  Assert(*b1 == *b1_copy);

  two_chain_list tc_list;
  tc_list.push_back(tc2);
  tc_list.push_back(tc1);

  vtol_block_sptr b2 = new vtol_block(tc_list);

  Assert(!(*b2 == *b1));
  Assert(*(b2->get_boundary_cycle()) == *tc2);

  vertex_list *verts = b2->vertices();

  Assert(verts->size()==8);

  delete verts;

  vtol_block_sptr b2_copy = new vtol_block(*b2);

  Assert(*(b2->get_boundary_cycle()) == *(b2_copy->get_boundary_cycle()));
  Assert(*b2 == *b2_copy);

  vsol_spatial_object_2d_sptr b2_clone = b2->clone();

  Assert(*b2 == *b2_clone);
  Assert(b2->cast_to_block()!=0);
  Assert(b1->valid_inferior_type(*tc1));
  Assert(tc1->valid_superior_type(*b1));

  vcl_cout << "Finished testing block\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
