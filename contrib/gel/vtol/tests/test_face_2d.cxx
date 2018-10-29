// This is gel/vtol/tests/test_face_2d.cxx
#include <testlib/testlib_test.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_2d_sptr.h>

static void test_face_2d()
{
  std::cout << "testing face 2d\n" << std::flush;

  vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(3.0,3.0);
  vtol_vertex_2d_sptr vh1 = new vtol_vertex_2d(5.0,5.0);
  vtol_vertex_2d_sptr vh2 = new vtol_vertex_2d(6.0,6.0);
  vtol_vertex_2d_sptr vh3 = new vtol_vertex_2d(7.0,7.0);

  vertex_list v_list;

  v_list.push_back(v1->cast_to_vertex());
  v_list.push_back(v2->cast_to_vertex());
  v_list.push_back(v3->cast_to_vertex());
  v_list.push_back(v4->cast_to_vertex());

  vtol_face_2d_sptr f1 = new vtol_face_2d(v_list);
  f1->describe(std::cout,8);

  vtol_edge_sptr e12 = new vtol_edge_2d(v1,v2);
  vtol_edge_sptr e23 = new vtol_edge_2d(v2,v3);
  vtol_edge_sptr e34 = new vtol_edge_2d(v3,v4);
  vtol_edge_sptr e41 = new vtol_edge_2d(v4,v1);

  vtol_edge_sptr eh12 = new vtol_edge_2d(vh1,vh2);
  vtol_edge_sptr eh23 = new vtol_edge_2d(vh2,vh3);
  vtol_edge_sptr eh31 = new vtol_edge_2d(vh3,vh1);

  edge_list e_list;

  e_list.push_back(e12);
  e_list.push_back(e23);
  e_list.push_back(e34);
  e_list.push_back(e41);

  vtol_one_chain_sptr oc1 = new vtol_one_chain(e_list);

  one_chain_list oc_list;
  oc_list.push_back(oc1);

  vtol_face_2d_sptr f2 = new vtol_face_2d(oc_list);
  f2->describe(std::cout,8);

  TEST("vtol_face_2d::get_one_chain(0)", f2->get_one_chain(0), oc1);
  TEST("vtol_face_2d::get_boundary_cycle()", f2->get_boundary_cycle(), oc1);

  edge_list he_list;

  he_list.push_back(eh12);
  he_list.push_back(eh23);
  he_list.push_back(eh31);

  vtol_one_chain_sptr oc1_hole = new vtol_one_chain(he_list);

  f2->add_hole_cycle(oc1_hole);

  one_chain_list *holes = f2->get_hole_cycles();
  TEST("vtol_face_2d::get_hole_cycles()", holes->size(), 1);
  TEST("vtol_face_2d::get_hole_cycles()", *((*holes)[0]), *oc1_hole);
  delete holes;

  vtol_face_2d_sptr f2c = new vtol_face_2d(f2);
  TEST("vtol_face_2d::f2c ==f2", *f2, *f2c);

  vertex_list f2c_all_verts; f2c->vertices(f2c_all_verts);
  for (auto & f2c_all_vert : f2c_all_verts)
    std::cout << "f2c_all_v (" << f2c_all_vert->cast_to_vertex_2d()->x() << ' '
             << f2c_all_vert->cast_to_vertex_2d()->y() << ")\n" << std::flush;

  vertex_list* f2c_verts = f2c->outside_boundary_vertices();
  for (auto & f2c_vert : *f2c_verts)
    std::cout << "f2c_bound_v (" << f2c_vert->cast_to_vertex_2d()->x() << ' '
             << f2c_vert->cast_to_vertex_2d()->y() << ")\n" << std::flush;
  TEST("vtol_face_2d::f2c outside_boundary_vertices()", f2c_verts->size(), 4);
  delete f2c_verts;
  holes = f2c->get_hole_cycles();
  TEST("vtol_face_2d::f2c get_hole_cycles()", holes->size(), 1);
  vertex_list f2c_hole_verts; (*holes)[0]->vertices(f2c_hole_verts);
  for (auto & f2c_hole_vert : f2c_hole_verts)
    std::cout << "f2c_hole_v (" << f2c_hole_vert->cast_to_vertex_2d()->x() << ' '
             << f2c_hole_vert->cast_to_vertex_2d()->y() << ")\n" << std::flush;
  TEST("vtol_face_2d::f2c hole_vertices()", f2c_hole_verts.size(), 3);
  delete holes;

  vtol_one_chain_sptr oce = new vtol_one_chain(e_list);
  vtol_one_chain_sptr oceh = new vtol_one_chain(he_list);
  one_chain_list f2e_chains;
  f2e_chains.push_back(oce);   f2e_chains.push_back(oceh);
  vtol_face_2d_sptr f2e = new vtol_face_2d(f2e_chains);

  vertex_list* f2e_verts = f2e->outside_boundary_vertices();
  for (auto & f2e_vert : *f2e_verts)
    std::cout << "f2e_v (" << f2e_vert->cast_to_vertex_2d()->x() << ' '
             << f2e_vert->cast_to_vertex_2d()->y() << ")\n" << std::flush;
  delete f2e_verts;
  holes = f2e->get_hole_cycles();
  vertex_list f2e_hole_verts; (*holes)[0]->vertices(f2e_hole_verts);
  for (auto & f2e_hole_vert : f2e_hole_verts)
    std::cout << "f2e_hole_v (" << f2e_hole_vert->cast_to_vertex_2d()->x() << ' '
             << f2e_hole_vert->cast_to_vertex_2d()->y() << ")\n" << std::flush;
  delete holes;

  one_chain_list f2e_out_chains; f2e->one_chains(f2e_out_chains);
  int ne = f2e_out_chains.size();
  TEST("f2e number of one_chains", ne, 2);

  edge_list f2e_edges; f2e->edges(f2e_edges);
  for (auto & f2e_edge : f2e_edges)
    std::cout << "f2e_edge[(" << f2e_edge->v1()->cast_to_vertex_2d()->x()
             << ' ' <<  f2e_edge->v1()->cast_to_vertex_2d()->y()
             << ")(" << f2e_edge->v2()->cast_to_vertex_2d()->x()
             << ' ' <<  f2e_edge->v2()->cast_to_vertex_2d()->y() << ")]\n"
             << std::flush;

  vtol_face_2d_sptr f2e_copy = new vtol_face_2d(f2e);
  edge_list f2e_copy_edges; f2e->edges(f2e_copy_edges);
  for (auto & f2e_copy_edge : f2e_copy_edges)
    std::cout << "f2e_edge_copy[(" << f2e_copy_edge->v1()->cast_to_vertex_2d()->x()
             << ' ' <<  f2e_copy_edge->v1()->cast_to_vertex_2d()->y()
             << ")(" << f2e_copy_edge->v2()->cast_to_vertex_2d()->x()
             << ' ' <<  f2e_copy_edge->v2()->cast_to_vertex_2d()->y() << ")]\n"
             << std::flush;


  vtol_face_2d_sptr f3 = new vtol_face_2d(oc1);
  f3->describe(std::cout,8);
  TEST("vtol_face_2d inequality", *f1 == *f3, false);
  TEST("vtol_face_2d inequality", *f1->cast_to_face() == *f3->cast_to_face(), false);

  vtol_face_2d_sptr f1_copy = new vtol_face_2d(f1);

  TEST("vtol_face_2d deep copy (pseudo copy constructor)", *f1, *f1_copy);
  TEST("vtol_face_2d::cast_to_face_2d()", f1->cast_to_face_2d()==nullptr, false);
  TEST("vtol_face_2d::cast_to_face()", *f1->cast_to_face(), *f1_copy->cast_to_face());

  vsol_spatial_object_2d_sptr f1_clone = f1->clone();
  TEST("vtol_face_2d::clone()", *f1, *f1_clone);
  f1_clone->describe(std::cout,8);

  vsol_region_2d_sptr surf = f1->surface();
  TEST("vtol_face_2d::surface()", surf.ptr()==nullptr, false);

  vtol_vertex_2d_sptr v5 = new vtol_vertex_2d(4.0,4.0);
  v_list.push_back(v5->cast_to_vertex());

  vtol_face_sptr new_f = new vtol_face_2d(v_list);

  TEST("vtol_face_2d inequality", *f1==*new_f, false);
  TEST("vtol_face_2d::cast_to_face()", f1->cast_to_face()==nullptr, false);
  TEST("vtol_face_2d::valid_inferior_type()", f1->valid_inferior_type(oc1), true);
  TEST("vtol_face_2d::valid_inferior_type()", f1->valid_inferior_type(e12->cast_to_topology_object()), false);
  TEST("vtol_one_chain::valid_superior_type()", oc1->valid_superior_type(f1), true);
  TEST("vtol_one_chain::valid_inferior_type()", oc1->valid_inferior_type(f1->cast_to_topology_object()), false);
  TEST("vtol_face_2d::get_num_edges()", f1->get_num_edges(), 4);
  TEST("vtol_face_2d::shares_edge_with()", f1->shares_edge_with(new_f), true);

  vertex_list *w_list = f1->outside_boundary_vertices();
  TEST("vtol_face::outside_boundary_vertices()", w_list->size(), 4);
  delete w_list;

  zero_chain_list *z_list = f1->outside_boundary_zero_chains();
  TEST("vtol_face::outside_boundary_zero_chains()", z_list->size(), 8);
  delete z_list;

  edge_list *ed_list = f1->outside_boundary_edges();
  TEST("vtol_face::outside_boundary_edges()", ed_list->size(), 4);
  delete ed_list;

  one_chain_list *o_list = f1->outside_boundary_one_chains();
  TEST("vtol_face::outside_boundary_one_chains()", o_list->size(), 1);
  delete o_list;
}

TESTMAIN(test_face_2d);
