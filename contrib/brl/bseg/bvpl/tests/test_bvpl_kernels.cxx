//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel.h>
#include <vnl/vnl_math.h>

#include <vul/vul_file.h>

//: Test changes
bool test_edge2d(vcl_string grid_path,unsigned height, unsigned width,vnl_vector_fixed<double,3> const& rotations )
{
  //create a kernel and save it to a grid
  bvpl_edge2d_kernel kernel;
  vcl_cout<< "alive\n";
  kernel.create(height, width, rotations);
  kernel.save_raw(grid_path);
  return true;
}

MAIN(test_bvpl_kernels)
{
  bool result = test_edge2d("grid1.raw", 3, 3, vnl_vector_fixed<double,3>(vnl_math::pi, 0.0, 0.0));
  TEST("grid 1", result, true);
#if 0
  //define variety of heights, widths, and rotations
  unsigned h1=3, h2=7, h3=12;
  double a1=2.0*vnl_math::pi/3.0, a2=vnl_math::pi/4.0, a3=vnl_math::pi/2.0, a4=vnl_math::pi;
  test_edge2d("grid2.vox", h1, h2, vnl_vector_fixed<double,3>(a4, a2, a3));
  test_edge2d("grid3.vox", h3, h2, vnl_vector_fixed<double,3>(a1, a4, 0));
#endif
  SUMMARY();
}

