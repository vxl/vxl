// This is gel/vtol/tests/test_vertex_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_algorithm.h> // vcl_find()
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vcl_list.h>

static void test_vertex_2d()
{
  // we want to test the methods on vtol_vertex_2d
  vcl_cout << "Testing vertex 2d\n";

  vtol_vertex_2d_sptr v1=new vtol_vertex_2d(1.0,2.0);
  v1->describe(vcl_cout,8);
  TEST("vtol_vertex_2d::x()", v1->x(), 1.0);
  TEST("vtol_vertex_2d::y()", v1->y(), 2.0);

  vsol_point_2d new_point(1.0,2.0);

  vtol_vertex_2d_sptr v1a = new vtol_vertex_2d(new_point);
  TEST("vtol_vertex_2d constructor from point", *v1, *v1a);

  vnl_double_2 vec; vec(0)=1.0; vec(1)=2.0;

  vtol_vertex_2d_sptr v1b = new vtol_vertex_2d(vec);
  TEST("vtol_vertex_2d constructor from vnl vector", *v1, *v1b);

  vtol_vertex_2d_sptr v1c = new vtol_vertex_2d(v1);
  TEST("vtol_vertex_2d copy constructor", *v1, *v1c);

  v1->set_x(2.0);
  v1->set_y(3.0);

  TEST("vtol_vertex_2d::set_x()", v1->x(), 2.0);
  TEST("vtol_vertex_2d::set_y()", v1->y(), 3.0);

  // try to clone this vertex

  vsol_spatial_object_2d_sptr so = v1->clone();
  TEST("vtol_vertex_2d::clone()", so?true:false, true);
  so->describe(vcl_cout,8);

  vtol_topology_object_sptr to = so->cast_to_topology_object();
  TEST("vtol_vertex_2d::clone()", to?true:false, true);
  to->describe(vcl_cout,8);
  vtol_vertex_sptr ve = to->cast_to_vertex();
  TEST("vtol_topology_object::cast_to_vertex()", ve?true:false, true);
  ve->describe(vcl_cout,8);
  vtol_vertex_2d_sptr v2 = ve->cast_to_vertex_2d();
  TEST("vtol_vertex::cast_to_vertex_2d()", v2?true:false, true);

  TEST("vtol_vertex_2d::x()", v2->x(), 2.0);
  TEST("vtol_vertex_2d::y()", v2->y(), 3.0);

  // try the point() accessor

  vsol_point_2d_sptr p = v2->point();
  TEST("vtol_vertex_2d::point()", p->x(), 2.0);
  TEST("vtol_vertex_2d::point()", p->y(), 3.0);

  // the == operator for spatial_object, topology_object, vertex and vertex_2d

  TEST("vtol_vertex_2d::operator==(vtol_vertex_2d)", *v1, *v2);
  TEST("vtol_vertex_2d::operator==(vsol_spatial_object_2d)", *v1, *so);
  TEST("vsol_spatial_object_2d::operator==(vtol_vertex_2d)", *so, *v1);
  TEST("vtol_vertex_2d::operator==(vtol_topology_object)", *v1, *to);
  TEST("vtol_topology_object::operator==(vtol_vertex_2d)", *to, *v1);
  TEST("vtol_vertex_2d::operator==(vtol_vertex)", *v1, *ve);
  TEST("vtol_vertex::operator==(vtol_vertex_2d)", *ve, *v1);
  TEST("vtol_vertex::operator==(vtol_topology_object)", *ve, *to);
  TEST("vtol_topology_object::operator==(vtol_vertex)", *to, *ve);
  TEST("vsol_spatial_object_2d::operator==(vtol_vertex)", *so, *ve);
  TEST("vtol_vertex::operator==(vsol_spatial_object_2d)", *ve, *so);
  TEST("vsol_spatial_object_2d::operator==(vtol_topology_object)", *so, *to);
  TEST("vtol_topology_object::operator==(vsol_spatial_object_2d)", *to, *so);

  // change the point with set_point() and verify it's different
  v2->set_point(new vsol_point_2d(4.0,5.0));
  TEST("vtol_vertex_2d::set_point()", v2->x()==4.0 && v2->y()==5.0, true);

  TEST("vtol_vertex_2d::operator!=(vtol_vertex_2d)", (*v1)==(*v2), false);
  TEST("vtol_vertex_2d::operator!=(vsol_spatial_object_2d)", (*v1)==(*so), false);
  TEST("vsol_spatial_object_2d::operator!=(vtol_vertex_2d)", (*so)==(*v1), false);
  TEST("vtol_vertex_2d::operator!=(vtol_topology_object)", (*v1)==(*to), false);
  TEST("vtol_topology_object::operator!=(vtol_vertex_2d)", (*to)==(*v1), false);
  TEST("vtol_vertex_2d::operator!=(vtol_vertex)", (*v1)==(*ve), false);
  TEST("vtol_vertex::operator!=(vtol_vertex_2d)", (*ve)==(*v1), false);

  // check casting

  TEST("vtol_vertex_2d::cast_to_vertex_2d()", v1->cast_to_vertex_2d()==0, false);

  // check distance from

  v1->set_x(1.0); v1->set_y(1.0);
  v2->set_x(4.0); v2->set_y(5.0);

  TEST("vtol_vertex_2d::euclidean_distance()", v1->euclidean_distance(*v2), 5.0);

  vec(0)=4.0; vec(1)=5.0;
  TEST("vtol_vertex_2d::v1->distance_from()", v1->distance_from(vec), 5.0);

  // checking the vertex side of things

  TEST("vtol_vertex_2d::topology_type()", v1->topology_type(), vtol_topology_object::VERTEX);

  vtol_edge_sptr new_edge = v1->new_edge(v2);
  edge_list* e_list=v1->edges();
  TEST("vtol_vertex_2d::edges()", e_list->size(), 1);
  TEST("vtol_vertex_2d::edges()", *((*e_list)[0]->v1()), *v1);
  TEST("vtol_vertex_2d::edges()", *((*e_list)[0]->v2()), *v2);
  delete e_list;

  vertex_list v_list;
  v1->explore_vertex(v_list);
  vcl_cout << "List size: " << v_list.size() << vcl_endl;
  for (unsigned int i=0; i<v_list.size(); ++i)
    vcl_cout << *(v_list[i]) << vcl_endl;
  TEST("vtol_vertex_2d::explore_vertex()", v_list.size(), 2);

  vtol_vertex_sptr v1v = v1->cast_to_vertex();
  vtol_vertex_sptr v2v = v2->cast_to_vertex();
  TEST("vtol_vertex equality", *v1v, *v1v);
  TEST("vtol_vertex inequality", (*v1v)==(*v2v), false);

  TEST("vtol_vertex::explore_vertex()", vcl_find(v_list.begin(),v_list.end(),v1v)==v_list.end(), false);
  TEST("vtol_vertex::explore_vertex()", vcl_find(v_list.begin(),v_list.end(),v2v)==v_list.end(), false);

  vtol_vertex_sptr v1v_copy = new vtol_vertex_2d(0.0,0.0);
  (*v1v_copy) = (*v1v);
  TEST("vtol_vertex assignment", *v1v_copy, *v1v);

  TEST("vtol_vertex::cast_to_vertex()", v1v->cast_to_vertex()==0, false);
  TEST("vtol_vertex::cast_to_vertex_2d()", v1v->cast_to_vertex_2d()==0, false);

  TEST("vtol_vertex::is_connected()", v1v->is_connected(*v2v), true);

  v1v_copy->cast_to_vertex_2d()->set_x(100.0);
  TEST("vtol_vertex::is_connected()", v1v->is_connected(*v1v_copy), false);

  TEST("vtol_vertex::is_endpoint()", v1v->is_endpoint(*new_edge), true);

  TEST("vtol_vertex::valid_superior_type()", v1v->valid_superior_type(new_edge->zero_chain()), true);
  TEST("vtol_one_chain::valid_inferior_type()", new_edge->zero_chain()->valid_inferior_type(v1v), true);
  TEST("vtol_vertex::valid_inferior_type()", v1v->valid_inferior_type(new_edge->cast_to_topology_object()), false);
  TEST("vtol_edge::valid_inferior_type()", new_edge->valid_inferior_type(v1v->cast_to_topology_object()), false);

  vcl_cout << "Testing superiors_list\n";
  vcl_cout << "ve before superiors access " << *ve << vcl_endl;
  const vcl_list<vtol_topology_object*>* sups = ve->superiors_list();
  TEST("vtol_vertex::superiors_list()", sups==0, false);
  for (vcl_list<vtol_topology_object*>::const_iterator sit = sups->begin();
       sit !=sups->end(); sit++)
  {
    vtol_zero_chain_sptr zc = (*sit)->cast_to_zero_chain();
    TEST("vtol_zero_chain::cast_to_zero_chain()", zc?true:false, true);
    vertex_list* verts = zc->vertices();
    bool found = false;
    for (vertex_list::iterator vit = verts->begin(); vit!=verts->end(); vit++)
    {
      vcl_cout << **vit;
      found = ve==*vit;
    }
    TEST("vtol_zero_chain::vertices()", found, true);
  }

  vcl_cout << "ve after superiors access " << *ve << vcl_endl;
}

TESTMAIN(test_vertex_2d);
