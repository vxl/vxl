// This is gel/vtol/tests/vtol_test_refcounting.cxx
#include <testlib/testlib_test.h>
//:
// \file
//  Verify correct refcounting of superiors/inferiors in a topology chain
// \author Peter Vanroose
// \date 9 December 2002

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_two_chain.h>
#include <vtol/vtol_block.h>

static void vtol_test_refcounting()
{
  // create an edge (3 ways)
  // 1. from 2 vertices
  vtol_vertex_2d* v1 = new vtol_vertex_2d(1.0,1.0); v1->set_id(1);
  TEST("single vertex has refcount 0", v1->get_references(), 0);
  vtol_vertex_2d* v2 = new vtol_vertex_2d(2.0,1.0); v2->set_id(2);
  vtol_edge_2d* e1 = new vtol_edge_2d(*v1,*v2); e1->set_id(1);
  TEST("vertex on edge has refcount 2", v1->get_references(), 2);
  // Note that refcounts of vertices on edges always jump by two, as opposed
  // to other topology objects: vertices are protected by both the 0-chain and
  // the edge, while strictly speaking only the 0-chain should protect them.
  TEST("single edge has refcount 0", e1->get_references(), 0);
  // 2. from a 0-chain
  vtol_vertex_2d* v3 = new vtol_vertex_2d(1.0,2.0); v3->set_id(3);
  vtol_zero_chain* z2 = new vtol_zero_chain(*v3,*v1);
  vtol_edge_2d* e2 = new vtol_edge_2d(*z2); e2->set_id(2);
  TEST("vertex on two edges has refcount 4", v1->get_references(), 4);
  // 3. from a 0-chain list
  vtol_zero_chain* z3 = new vtol_zero_chain(*v2,*v3);
  zero_chain_list zl3; zl3.push_back(z3);
  vtol_edge_2d* e3 = new vtol_edge_2d(zl3); e3->set_id(3);
  zl3.clear(); // remove 0-chain from list, to avoid "false" refcounts
  TEST("vertex on two edges has refcount 4", v2->get_references(), 4);
  TEST("vertex on two edges has refcount 4", v3->get_references(), 4);
  TEST("single edge has refcount 0", e1->get_references(), 0);
  TEST("single edge has refcount 0", e2->get_references(), 0);
  TEST("single edge has refcount 0", e3->get_references(), 0);

  v1->describe(vcl_cout,8);
  e1->describe(vcl_cout,8);

  // create a triangle (face) from 3 vertices
  vertex_list vl1; vl1.push_back(v1); vl1.push_back(v2); vl1.push_back(v3);
  vtol_face_2d* f1 = new vtol_face_2d(vl1); f1->set_id(1);
  vtol_one_chain* oc1 = f1->get_one_chain();
  vl1.clear(); // remove vertices from list, to avoid "false" refcounts
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);
  TEST("1-chain of triangle has refcount 1", oc1->get_references(), 1);
  TEST("single face has refcount 0", f1->get_references(), 0);

  v1->describe(vcl_cout,8);
  e1->describe(vcl_cout,8);
  oc1->describe(vcl_cout,8);
  f1->describe(vcl_cout,8);

  // create a rectangle (face) from 4 vertices
  vl1.push_back(new vtol_vertex_2d(0.0,0.0));
  vl1.push_back(new vtol_vertex_2d(3.0,0.0));
  vl1.push_back(new vtol_vertex_2d(0.0,3.0));
  vl1.push_back(new vtol_vertex_2d(3.0,3.0));
  vtol_face_2d* f2 = new vtol_face_2d(vl1); f2->set_id(2);
  vl1.clear();
  TEST("single face has refcount 0", f2->get_references(), 0);
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);

  f2->describe(vcl_cout,8);

  // create a face with a hole from 2 1-chains
  one_chain_list ol1;
  ol1.push_back(f1->inferiors()->front()->cast_to_one_chain());
  ol1.push_back(f2->inferiors()->front()->cast_to_one_chain());
  vtol_face_2d* f3 = new vtol_face_2d(ol1); f3->set_id(3);
  ol1.clear();
  TEST("single face has refcount 0", f3->get_references(), 0);
  // This new face should not increment the refcounts 2 levels down:
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);

  f3->describe(vcl_cout,8);

  // Now removing the objects one by one, and watching the refcounts:
  delete f1;
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);
  delete f2;
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);
  // before deleting the last face, protect the edges from being deleted:
  e1->ref(); e2->ref(); e3->ref();
  delete f3;
  TEST("vertex on 2 edges has refcount 4", v1->get_references(), 4);
  TEST("vertex on 2 edges has refcount 4", v2->get_references(), 4);
  TEST("vertex on 2 edges has refcount 4", v3->get_references(), 4);
  TEST("single edge has refcount 0", e1->get_references(), 1);
  TEST("single edge has refcount 0", e2->get_references(), 1);
  TEST("single edge has refcount 0", e3->get_references(), 1);
  v1->ref(); v2->ref(); v3->ref();
  e1->unref(); // this will delete e1;
  TEST("vertex on 1 edge has refcount 2", v1->get_references(), 3);
  TEST("vertex on 1 edge has refcount 2", v2->get_references(), 3);
  TEST("vertex on 2 edges has refcount 4", v3->get_references(), 5);
  TEST("single edge has refcount 0", e2->get_references(), 1);
  TEST("single edge has refcount 0", e3->get_references(), 1);
  e2->unref(); // this will delete e2;
  TEST("single vertex has refcount 0", v1->get_references(), 1);
  TEST("vertex on 1 edge has refcount 2", v2->get_references(), 3);
  TEST("vertex on 1 edge has refcount 2", v3->get_references(), 3);
  TEST("single edge has refcount 0", e3->get_references(), 1);
  e3->unref(); // this will delete e3;
  TEST("single vertex has refcount 0", v1->get_references(), 1);
  TEST("single vertex has refcount 0", v2->get_references(), 1);
  TEST("single vertex has refcount 0", v3->get_references(), 1);
  v1->unref(); v2->unref(); v3->unref(); // no memory leaks left
}

TESTLIB_DEFINE_MAIN(vtol_test_refcounting);
