#include <vcl_fstream.h>

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  // we want to test the methods on vtol_vertex_2d
  vcl_cout << "Testing vertex 2d" << vcl_endl;

  vtol_vertex_2d_sptr v1=new vtol_vertex_2d(1.0,2.0);
  Assert(v1->x()==1.0);
  Assert(v1->y()==2.0);

  vsol_point_2d new_point(1.0,2.0);

  vtol_vertex_2d_sptr v1a = new vtol_vertex_2d(new_point);

  vnl_double_2 vec;
  vec(0)=1.0;
  vec(1)=2.0;

   vtol_vertex_2d_sptr v1b = new vtol_vertex_2d(vec);

   vtol_vertex_2d_sptr v1c = new vtol_vertex_2d(*v1);

   // test equivalency of constructors

   Assert(((*v1)==(*v1a)) && ((*v1)==(*v1b)) && ((*v1)==(*v1c)));


  v1->set_x(2.0);
  v1->set_y(3.0);

  Assert(v1->x()==2.0);
  Assert(v1->y()==3.0);

  // try to clone this vertex

  vsol_spatial_object_2d_sptr so = v1->clone();

  vtol_topology_object_sptr to = so->cast_to_topology_object();
  Assert(to);
  vtol_vertex_sptr ve = to->cast_to_vertex();
  Assert(ve);
  vtol_vertex_2d_sptr v2 = ve->cast_to_vertex_2d();
  Assert(v2);

  Assert(v2->x()==2.0);
  Assert(v2->y()==3.0);

  // try the point() accessor

  vsol_point_2d_sptr p = v2->point();
  Assert(p->x()==2.0);
  Assert(p->y()==3.0);

  // the == operator for spatial_object, topology_object, vertex and vertex_2d

  Assert((*v1)==(*v2));
  Assert((*v1)==(*so));
  Assert((*so)==(*v1));
  Assert((*v1)==(*to));
  Assert((*to)==(*v1));
  Assert((*v1)==(*ve));
  Assert((*ve)==(*v1));
  Assert((*ve)==(*to));
  Assert((*to)==(*ve));
  Assert((*so)==(*ve));
  Assert((*ve)==(*so));
  Assert((*so)==(*to));
  Assert((*to)==(*so));

  // change the point with set_point() and verify it's different
  vsol_point_2d_sptr np = new vsol_point_2d(4.0,5.0);
  v2->set_point(np);
  Assert(v2->x()==4.0 && v2->y()==5.0);

  Assert(!((*v1)==(*v2)));
  Assert(!((*v1)==(*so)));
  Assert(!((*so)==(*v1)));
  Assert(!((*v1)==(*to)));
  Assert(!((*to)==(*v1)));
  Assert(!((*v1)==(*ve)));
  Assert(!((*ve)==(*v1)));

  // check casting

  Assert(v1->cast_to_vertex_2d()!=0);

  // check distance from

  v1->set_x(1.0);
  v1->set_y(1.0);
  v2->set_x(4.0);
  v2->set_y(5.0);

  Assert(v1->euclidean_distance(*v2)==5.0);
  vec(0)=4.0;
  vec(1)=5.0;

  Assert(v1->distance_from(vec)==5.0);

  // checking the vertex side of things

  Assert(v1->topology_type()==vtol_topology_object::VERTEX);

  vtol_edge_sptr new_edge = v1->new_edge(*v2);
  edge_list* e_list=v1->edges();
  Assert(e_list->size()==1);
  Assert((*e_list)[0]->v1()==v1);
  Assert((*e_list)[0]->v2()==v2);
  delete e_list;

  vertex_list v_list;

  v1->explore_vertex(v_list);
  vcl_cout << "List size: " << v_list.size() << vcl_endl;
  for (unsigned int i=0; i<v_list.size(); ++i)
    vcl_cout << *(v_list[i]) << vcl_endl;
  Assert(v_list.size()==2);

  vtol_vertex_sptr v1v = v1->cast_to_vertex();
  vtol_vertex_sptr v2v = v2->cast_to_vertex();

  // Assert(vcl_find(v_list.begin(),v_list.end(),v1v)!=v_list.end());
  // Assert(vcl_find(v_list.begin(),v_list.end(),v2v)!=v_list.end());

  Assert((*v1v)==(*v1v));
  Assert((*v1v)!=(*v2v));

  vtol_vertex_sptr v1v_copy = new vtol_vertex_2d(0.0,0.0);
  (*v1v_copy) = (*v1v);

  Assert((*v1v_copy)==(*v1v));

  Assert(v1v->cast_to_vertex()!=0);
  Assert(v1v->cast_to_vertex_2d()!=0);

  Assert(v1v->is_connected(*v2v));

  v1v_copy->cast_to_vertex_2d()->set_x(100.0);
  Assert(!(v1v->is_connected(*v1v_copy)));

  Assert(v1v->is_endpoint((*new_edge)));

  Assert(v1v->valid_superior_type(*(new_edge->zero_chain())));
  Assert(!(v1v->valid_superior_type(*(new_edge))));
  Assert(!(v1v->valid_inferior_type(*(new_edge))));

  vcl_cout << "finished testing vertex 2d" << vcl_endl;
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
