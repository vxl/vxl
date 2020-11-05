// This is core/vgl/io/tests/test_pointset_3d_io.cxx

#include <iostream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "testlib/testlib_test.h"
#include "vgl/vgl_point_3d.h"
#include "vpl/vpl.h"
#include "vsl/vsl_indent.h"
#include <vgl/io/vgl_io_pointset_3d.h>


template <class Type>
void test_pointset_3d_io_templated()
{
  std::cout << "**************************\n"
            << "Testing vgl_pointset_3d_io\n"
            << "**************************\n";

  std::vector<vgl_point_3d<Type> > points;
  std::vector<vgl_vector_3d<Type>> normals;
  std::vector<Type> scalars;

  for (size_t i=0; i < 10; i++) {
    points.push_back(vgl_point_3d<Type>(i, i, i));
    normals.push_back(vgl_vector_3d<Type>(i, i, i));
    scalars.push_back(i);
  }

  // construct pointsets with all combinations of points, normals, scalars
  vgl_pointset_3d<Type> ptset_out_points(points), ptset_in_points;
  vgl_pointset_3d<Type> ptset_out_points_normals(points, normals), ptset_in_points_normals;
  vgl_pointset_3d<Type> ptset_out_points_scalars(points, scalars), ptset_in_points_scalars;
  vgl_pointset_3d<Type> ptset_out_points_normals_scalars(points, normals, scalars), ptset_in_points_normals_scalars;

  // === test pointset with only points ===
  vsl_b_ofstream bfs_out("vgl_pointset_3d_points_test_io.bvl.tmp");
  TEST("Created vgl_pointset_3d_points_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, ptset_out_points);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vgl_pointset_3d_points_test_io.bvl.tmp");
  TEST("Opened vgl_pointset_3d_points_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, ptset_in_points);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink("vgl_pointset_3d_points_test_io.bvl.tmp");

  TEST("ptset_out == ptset_in", ptset_out_points == ptset_in_points, true);

  vsl_print_summary(std::cout, ptset_out_points);
  std::cout << std::endl;
  vsl_indent_clear_all_data();

  // === test pointset with points and normals ===
  bfs_out = vsl_b_ofstream("vgl_pointset_3d_points_normals_test_io.bvl.tmp");
  TEST("Created vgl_pointset_3d_points_normals_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, ptset_out_points_normals);
  bfs_out.close();

  bfs_in = vsl_b_ifstream("vgl_pointset_3d_points_normals_test_io.bvl.tmp");
  TEST("Opened vgl_pointset_3d_points_normals_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, ptset_in_points_normals);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink("vgl_pointset_3d_points_normals_test_io.bvl.tmp");

  TEST("ptset_out == ptset_in", ptset_out_points_normals == ptset_in_points_normals, true);

  vsl_print_summary(std::cout, ptset_out_points_normals);
  std::cout << std::endl;
  vsl_indent_clear_all_data();

  // === test pointset with points and scalars ===
  bfs_out = vsl_b_ofstream("vgl_pointset_3d_points_scalars_test_io.bvl.tmp");
  TEST("Created vgl_pointset_3d_points_scalars_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, ptset_out_points_scalars);
  bfs_out.close();

  bfs_in = vsl_b_ifstream("vgl_pointset_3d_points_scalars_test_io.bvl.tmp");
  TEST("Opened vgl_pointset_3d_points_scalars_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, ptset_in_points_scalars);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink("vgl_pointset_3d_points_scalars_test_io.bvl.tmp");

  TEST("ptset_out == ptset_in", ptset_out_points_scalars == ptset_in_points_scalars, true);

  vsl_print_summary(std::cout, ptset_out_points_scalars);
  std::cout << std::endl;
  vsl_indent_clear_all_data();

  // === test pointset with points, normals, and scalars ===
  bfs_out = vsl_b_ofstream("vgl_pointset_3d_points_normals_scalars_test_io.bvl.tmp");
  TEST("Created vgl_pointset_3d_points_normals_scalars_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, ptset_out_points_normals_scalars);
  bfs_out.close();

  bfs_in = vsl_b_ifstream("vgl_pointset_3d_points_normals_scalars_test_io.bvl.tmp");
  TEST("Opened vgl_pointset_3d_points_normals_scalars_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, ptset_in_points_normals_scalars);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink("vgl_pointset_3d_points_normals_scalars_test_io.bvl.tmp");

  TEST("ptset_out == ptset_in", ptset_out_points_normals_scalars == ptset_in_points_normals_scalars, true);

  vsl_print_summary(std::cout, ptset_out_points_normals_scalars);
  std::cout << std::endl;
  vsl_indent_clear_all_data();
}


void
test_pointset_3d_io()
{
  test_pointset_3d_io_templated<float>();
  test_pointset_3d_io_templated<double>();
}

TESTMAIN(test_pointset_3d_io);
