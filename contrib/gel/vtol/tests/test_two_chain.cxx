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
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_two_chain_sptr.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  vcl_cout << "testing two chain" << vcl_endl;

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

  vcl_vector<signed char> dirs;

  dirs.push_back(-1);
  dirs.push_back(1);

  vtol_two_chain_sptr tc2 = new vtol_two_chain(f_list1,dirs);

  vtol_two_chain_sptr tc1_copy = new vtol_two_chain(*tc1);

  Assert(*tc1 == *tc1_copy);

  Assert(!(*tc1 == *tc2));

  vsol_spatial_object_3d_sptr tc2_clone = tc2->clone();

  Assert(*tc2 == *tc2_clone);

  Assert(tc1->topology_type() == vtol_topology_object::TWOCHAIN);

  Assert(tc2->direction(*f1) == -1);
  Assert(tc2->direction(*f2) == 1);

  Assert(tc1->face(0)==f1.ptr());

  vtol_vertex_2d_sptr v9 = new vtol_vertex_2d(1.0,0.0);
  vtol_vertex_2d_sptr v10 = new vtol_vertex_2d(2.0,1.0);
  vtol_vertex_2d_sptr v11 = new vtol_vertex_2d(3.0,2.0);
  vtol_vertex_2d_sptr v12 = new vtol_vertex_2d(4.0,3.0);

  vertex_list v_list3;

  v_list3.push_back(v9->cast_to_vertex());
  v_list3.push_back(v10->cast_to_vertex());
  v_list3.push_back(v11->cast_to_vertex());
  v_list3.push_back(v12->cast_to_vertex());

  vtol_face_2d_sptr f3 = new vtol_face_2d(v_list3);

  tc1->add_face(*f3,1);

  Assert(tc1->face(2) == f3.ptr());

  tc1->remove_face(*f2);

  Assert(tc1->face(1) == f3.ptr());
  Assert(tc1->cast_to_two_chain()!=0);
  Assert(tc1->valid_inferior_type(*f1));
  Assert(f1->valid_superior_type(*tc1));
  Assert(tc1->valid_chain_type(*tc1));
  Assert(tc1->num_faces()==2);

  tc1->link_chain_inferior(*tc2);

  vtol_two_chain_sptr new_copy = new vtol_two_chain(*tc1);
  Assert(*new_copy == *tc1);

  vcl_cout << "Finished testing two chain " << vcl_endl;
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
