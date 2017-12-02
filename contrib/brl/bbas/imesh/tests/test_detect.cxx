#include <testlib/testlib_test.h>
#include <imesh/imesh_detection.h>
#include "test_share.h"

static void test_detect()
{
  imesh_mesh cube1, cube2, cube3;
  make_cube(cube1);
  make_cube(cube2);
  make_cube(cube3);

  cube1.merge(cube2);
  cube1.merge(cube3);

  cube1.build_edge_graph();

  std::vector<std::set<unsigned int> > components =
     imesh_detect_connected_components(cube1.half_edges());

  TEST("Found all connected components", components.size(), 3);

  std::set<unsigned int> group1 =
      imesh_detect_connected_faces(cube1.half_edges(), 0);
  std::set<unsigned int> group2 =
      imesh_detect_connected_faces(cube1.half_edges(), 3);
  std::set<unsigned int> group3 =
      imesh_detect_connected_faces(cube1.half_edges(), 9);

  TEST("Connected faces have correct number", group1.size(), 6);
  TEST("Connected faces different starting point", group1, group2);
  TEST("Connected faces finds two groups", group1 != group3, true);
}

TESTMAIN(test_detect);
