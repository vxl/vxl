//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>

#include <rgtl/rgtl_octree_objects.h>
#include <rgtl/rgtl_object_array_points.h>
#include <rgtl/rgtl_octree_cell_bounds.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_octree_objects()
{
  // Default bounds are unit cube.
  rgtl_octree_cell_bounds<3> bounds;

  // Initialize with a fixed seed so test is reproducible.
  vnl_random mz_random;
  mz_random.reseed(123456);

  // Setup a timer.
  vul_timer timer;

  // Generate points in the unit cube.
  unsigned int const n = 10000;
  rgtl_object_array_points<3> points(n);
  for (unsigned int i=0; i < n; ++i)
  {
    double x[3];
    x[0] = mz_random.drand32(0,1);
    x[1] = mz_random.drand32(0,1);
    x[2] = mz_random.drand32(0,1);
    points.set_point(i, x);
  }

  // Construct the spatial structure.
  testlib_test_begin("construct");
  timer.mark();
  rgtl_octree_objects<3> objects(points, bounds, 5);
  double ct_ms = timer.user();
  testlib_test_perform(true);

  // Compute the distance transform in the spatial structure.
  testlib_test_begin("distance transform (order 4)");
  timer.mark();
  bool df_worked = objects.compute_distance_transform(4);
  double df_ms = timer.user();
  testlib_test_perform(df_worked);

  // Query the structure.
  int count;
  double p[3] = {0.5, 0.5, 0.5};
  double closest_distances_squared[4];
  int closest_ids[4];

  testlib_test_begin("query closest");
  count = objects.query_closest(p, 4, closest_ids,
                                closest_distances_squared, 0);
  testlib_test_perform(count == 4);

  double radius = std::sqrt(closest_distances_squared[count-1]);
  std::vector<int> sphere_ids;
  testlib_test_begin("query sphere");
  count = objects.query_sphere(p, radius, sphere_ids);
  testlib_test_perform(count == 4);

  std::cout << "  octree construction took " << (ct_ms*0.001) << "s\n"
           << "  distance transform took " << (df_ms*0.001) << 's' << std::endl;
}

TESTMAIN(test_octree_objects);
