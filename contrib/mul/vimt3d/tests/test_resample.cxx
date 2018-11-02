// This is mul/vimt3d/tests/test_resample.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vimt3d/vimt3d_resample_trilinear.h>
#include <vimt3d/vimt3d_transform_3d.h>


//========================================================================
static void test_resample_trilinear_scale_2()
{
  std::cout << "*******************************************\n"
           << " Testing vimt3d_resample_trilinear_scale_2\n"
           << "*******************************************\n";

  unsigned sni=3;
  unsigned snj=3;
  unsigned snk=3;
  vimt3d_image_3d_of<int> src(sni, snj, snk);
  for (unsigned i=0; i<sni; ++i)
    for (unsigned j=0; j<snj; ++j)
      for (unsigned k=0; k<snk; ++k)
        src.image()(i,j,k) = 10000 + 10*k + 100*j + 1000*i;

  vimt3d_image_3d_of<int> dst;
  vimt3d_resample_trilinear_scale_2(src, dst);
  unsigned dni = dst.image().ni();
  unsigned dnj = dst.image().nj();
  unsigned dnk = dst.image().nk();


  // Testing
//  src.print_all(std::cout);
//  dst.print_all(std::cout);
  ///

  bool all_voxs_correct = true;
  for (unsigned i=0; i<dni; ++i)
    for (unsigned j=0; j<dnj; ++j)
      for (unsigned k=0; k<dnk; ++k)
        all_voxs_correct = all_voxs_correct &&
          dst.image()(i,j,k)==int(10000 + 5*k + 50*j + 500*i);

  TEST("Voxel values correct", all_voxs_correct, true);

  const vimt3d_transform_3d& src_w2i = src.world2im();
  const vimt3d_transform_3d& dst_w2i = dst.world2im();

  vimt3d_transform_3d scaling;
  scaling.set_zoom_only(2.0, 2.0, 2.0, 0.0, 0.0, 0.0);
  TEST("Transforms correct", dst_w2i==(scaling*src_w2i), true);
}



//========================================================================
static void test_resample_trilin_smoothing_edge_extend()
{
  std::cout << "******************************************************\n"
           << " Testing vimt3d_resample_trilin_smoothing_edge_extend\n"
           << "******************************************************\n";

  unsigned sni=5;
  unsigned snj=5;
  unsigned snk=5;
  vimt3d_image_3d_of<int> src(sni, snj, snk);
  for (unsigned i=0; i<sni; ++i)
    for (unsigned j=0; j<snj; ++j)
      for (unsigned k=0; k<snk; ++k)
        src.image()(i,j,k) = 10000 + 10*k + 100*j + 1000*i;

  vimt3d_transform_3d w2i;
  w2i.set_zoom_only(0.5, 0, 0, 1);
  vimt3d_image_3d_of<int> dst(3, 3, 4, 1, w2i);
  vimt3d_resample_trilin_smoothing_edge_extend(src, dst);
  unsigned dni = dst.image().ni();
  unsigned dnj = dst.image().nj();
  unsigned dnk = dst.image().nk();


  // Testing
 // src.print_all(std::cout);
 // dst.print_all(std::cout);
  ///

  bool all_voxs_correct = true;
  for (unsigned i=0; i<dni; ++i)
    for (unsigned j=0; j<dnj; ++j)
      for (unsigned k=0; k<dnk; ++k)
        all_voxs_correct = all_voxs_correct && (
          (k==0 && dst.image()(i,j,k)==int(10000 + 0        + 200*j + 2000*i)) ||
          (k>0  && dst.image()(i,j,k)==int(10000 + 20*(k-1) + 200*j + 2000*i)) );

  TEST("Voxel values correct", all_voxs_correct, true);

  TEST("Transforms correct", dst.world2im(), w2i);
}


//========================================================================
//========================================================================
static void test_resample()
{
  test_resample_trilinear_scale_2();
  test_resample_trilin_smoothing_edge_extend();
}


//========================================================================
//========================================================================
TESTMAIN(test_resample);
