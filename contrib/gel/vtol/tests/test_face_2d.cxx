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

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  vcl_cout << "testing face 2d" << vcl_endl;

  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(3.0,3.0);

  vertex_list v_list;

  v_list.push_back(v1->cast_to_vertex());
  v_list.push_back(v2->cast_to_vertex());
  v_list.push_back(v3->cast_to_vertex());
  v_list.push_back(v4->cast_to_vertex());

  vtol_face_2d_sptr f1 = new vtol_face_2d(v_list);

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

  one_chain_list oc_list;
  oc_list.push_back(oc1);

  vtol_face_2d_sptr f2 = new vtol_face_2d(oc_list);

  Assert(f2->get_one_chain()==oc1.ptr());
  Assert(f2->get_boundary_cycle()==oc1.ptr());

  edge_list he_list;

  e_list.push_back(e12);
  e_list.push_back(e23);
  e_list.push_back(e34);

  vtol_one_chain_sptr oc1_hole = new vtol_one_chain(he_list);

  f2->add_hole_cycle(*oc1_hole);

  vcl_vector<vtol_one_chain_sptr> *holes = f2->get_hole_cycles();

  Assert(holes->size()==1);
  Assert(*((*holes)[0])==(*oc1_hole));

  delete holes;

  vtol_face_2d_sptr f3 = new vtol_face_2d(*oc1);

  Assert(*f1 != *f3);

  vtol_face_2d_sptr f1_copy = new vtol_face_2d(*f1);

  Assert(*f1 == *f1_copy);

  vsol_spatial_object_2d_sptr f1_clone = f1->clone();
  Assert(*f1 == *f1_clone);

  vsol_region_2d_sptr surf = f1->surface();
  Assert(surf.ptr()!=0);

  Assert(f1->cast_to_face_2d()!=0);

  vtol_vertex_2d_sptr v5 = new vtol_vertex_2d(4.0,4.0);
  v_list.push_back(v5->cast_to_vertex());

  vtol_face_2d_sptr new_f = new vtol_face_2d(v_list);

  Assert(!((*f1==*new_f)));
  Assert(f1->topology_type()==vtol_topology_object::FACE);
  Assert(f1->cast_to_face()!=0);
  Assert(f1->valid_inferior_type(*oc1));
  Assert(!(f1->valid_inferior_type(*e12)));
  Assert(f1->get_num_edges()==4);
  Assert(f1->shares_edge_with(*new_f));

  vcl_cout << "Finished testing face 2d " << vcl_endl;
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
