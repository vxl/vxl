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
  auto* v1 = new vtol_vertex_2d(1.0,1.0); v1->set_id(1);
  TEST("single vertex has refcount 0", v1->get_references(), 0);
  auto* v2 = new vtol_vertex_2d(2.0,1.0); v2->set_id(2);
  vtol_edge_2d* e1 = new vtol_edge_2d(vtol_vertex_2d_sptr(v1),v2); e1->set_id(1);
  TEST("vertex on edge has refcount 2", v1->get_references(), 2);
  // Note that refcounts of vertices on edges always jump by two, as opposed
  // to other topology objects: vertices are protected by both the 0-chain and
  // the edge, while strictly speaking only the 0-chain should protect them.
  TEST("single edge has refcount 0", e1->get_references(), 0);
  // 2. from a 0-chain
  auto* v3 = new vtol_vertex_2d(1.0,2.0); v3->set_id(3);
  vtol_zero_chain* z2 = new vtol_zero_chain(vtol_vertex_2d_sptr(v3),vtol_vertex_2d_sptr(v1));
  vtol_edge_2d* e2 = new vtol_edge_2d(z2); e2->set_id(2);
  TEST("vertex on two edges has refcount 4", v1->get_references(), 4);
  // 3. from a 0-chain list
  vtol_zero_chain* z3 = new vtol_zero_chain(vtol_vertex_2d_sptr(v2),vtol_vertex_2d_sptr(v3));
  zero_chain_list zl3; zl3.push_back(z3);
  auto* e3 = new vtol_edge_2d(zl3); e3->set_id(3);
  zl3.clear(); // remove 0-chain from list, to avoid "false" refcounts
  TEST("vertex on two edges has refcount 4", v2->get_references(), 4);
  TEST("vertex on two edges has refcount 4", v3->get_references(), 4);
  TEST("single edge has refcount 0", e1->get_references(), 0);
  TEST("single edge has refcount 0", e2->get_references(), 0);
  TEST("single edge has refcount 0", e3->get_references(), 0);

  v1->describe(std::cout,8);
  e1->describe(std::cout,8);

  // create a triangle (face) from 3 vertices
  vertex_list vl1; vl1.push_back(v1); vl1.push_back(v2); vl1.push_back(v3);
  auto* f1 = new vtol_face_2d(vl1); f1->set_id(1);
  vtol_one_chain* oc1 = f1->get_one_chain()->cast_to_one_chain();
  vl1.clear(); // remove vertices from list, to avoid "false" refcounts
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);
  TEST("1-chain of triangle has refcount 1", oc1->get_references(), 1);
  TEST("single face has refcount 0", f1->get_references(), 0);

  v1->describe(std::cout,8);
  e1->describe(std::cout,8);
  oc1->describe(std::cout,8);
  f1->describe(std::cout,8);

  // create a rectangle (face) from 4 vertices
  vl1.push_back(new vtol_vertex_2d(0.0,0.0));
  vl1.push_back(new vtol_vertex_2d(3.0,0.0));
  vl1.push_back(new vtol_vertex_2d(0.0,3.0));
  vl1.push_back(new vtol_vertex_2d(3.0,3.0));
  auto* f2 = new vtol_face_2d(vl1); f2->set_id(2);
  vl1.clear();
  TEST("single face has refcount 0", f2->get_references(), 0);
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);

  f2->describe(std::cout,8);

  // create a face with a hole from 2 1-chains
  one_chain_list ol1;
  ol1.push_back(f1->inferiors()->front()->cast_to_one_chain());
  ol1.push_back(f2->inferiors()->front()->cast_to_one_chain());
  auto* f3 = new vtol_face_2d(ol1); f3->set_id(3);
  ol1.clear();
  TEST("single face has refcount 0", f3->get_references(), 0);
  // This new face should not increment the refcounts 2 levels down:
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);

  f3->describe(std::cout,8);

  // create a block from the 3 faces created above
  face_list fl1;
  fl1.push_back(f1); fl1.push_back(f2); fl1.push_back(f3);
  auto* b1 = new vtol_block(fl1); b1->ref();
  fl1.clear();
  TEST("single block has refcount 0", b1->get_references(), 1);
  // This new block should not increment the refcounts 2 levels down:
  TEST("face on block has refcount 1", f1->get_references(), 1);
  TEST("face on block has refcount 1", f2->get_references(), 1);
  TEST("face on block has refcount 1", f3->get_references(), 1);

  // create a block from a two-chain
  fl1.push_back(f1); fl1.push_back(f2); fl1.push_back(f3);
  vtol_two_chain* tc1 = f1->superiors_list()->front()->cast_to_two_chain();
  TEST("non-shared 2-chain has refcount 1", tc1->get_references(), 1);
  two_chain_list tl1; tl1.push_back(tc1);
  auto* b2 = new vtol_block(tl1); b2->ref();
  fl1.clear(); tl1.clear();
  TEST("single block has refcount 0", b2->get_references(), 1);
  TEST("other block still has refcount 0", b1->get_references(), 1);
  // This new block should not increment the refcounts 2 levels down:
  TEST("face on block has refcount 1", f1->get_references(), 1);
  TEST("face on block has refcount 1", f2->get_references(), 1);
  TEST("face on block has refcount 1", f3->get_references(), 1);
  // But it should increment the refcount of the shared 2-chain:
  TEST("shared 2-chain has refcount 2", tc1->get_references(), 2);

  // The two blocks should be identical:
  TEST("blocks are identical", *b1, *b2);
  TEST("single block still has refcount 0", b1->get_references(), 1);
  TEST("single block still has refcount 0", b2->get_references(), 1);

  // Now removing the objects one by one, and watching the refcounts:
  b1->unref();
  TEST("face on block has refcount 1", f1->get_references(), 1);
  TEST("face on block has refcount 1", f2->get_references(), 1);
  TEST("face on block has refcount 1", f3->get_references(), 1);
  TEST("non-shared 2-chain has refcount 1", tc1->get_references(), 1);
  // before deleting the last block, protect the faces from being deleted;
  // the 2-chains are automatically deleted, so no memory leaks.
  f1->ref(); f2->ref(); f3->ref(); b2->unref();
  // And the same thing for the faces:
  f1->unref();
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);
  f2->unref();
  TEST("vertex on triangle has refcount 4", v1->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v2->get_references(), 4);
  TEST("vertex on triangle has refcount 4", v3->get_references(), 4);
  TEST("edge on triangle has refcount 1", e1->get_references(), 1);
  TEST("edge on triangle has refcount 1", e2->get_references(), 1);
  TEST("edge on triangle has refcount 1", e3->get_references(), 1);
  // before deleting the last face, protect the edges from being deleted:
  e1->ref(); e2->ref(); e3->ref();
  f3->unref();
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

TESTMAIN(vtol_test_refcounting);
