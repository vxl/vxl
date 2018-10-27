//:
// \file
// Created by Isabel Restrepo on 8/26/09.
//
//
#include <testlib/testlib_test.h>
#include <bvpl/kernels/bvpl_create_directions.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>

template <class FUNC>
bool test_directions(FUNC dir)
{
  bvpl_edge3d_kernel_factory kernels_3d(-1,1,-1,1,-1,1);
  bvpl_kernel_vector_sptr kernel_vec = kernels_3d.create_kernel_vector(dir);

  std::vector<vnl_float_3> axes1= dir.get_axes();

  auto iter1 = axes1.begin();
  auto iter2 = kernel_vec->begin();
  bool result = true;
  for (; iter1!=axes1.end(); ++iter1, ++iter2)
  {
    result = result && ((*iter1 -(*iter2)->axis()).two_norm() < 1e-2f);
    if (!result)
      std::cout << *iter1 << "  should be  " << (*iter2)->axis() << std::endl;
  }

  return result;
}

//: Test that directions created by functors are in fact stored in the kernels
static void test_vector_directions()
{
  bvpl_create_directions_a dir1;
  TEST("Direction a", true, test_directions(dir1));
  bvpl_create_directions_b dir2;
  TEST("Direction b", true, test_directions(dir2));
  bvpl_all_corner_dirs dir3;
  TEST("Direction c", true, test_directions(dir3));
}

TESTMAIN(test_vector_directions);
