// This is mul/vil3d/tests/test_resample.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/vil3d_resample_simple.h>
#include <vil3d/vil3d_resample_trilinear.h>
#include <vil3d/vil3d_print.h>


//==================================================================================
//==================================================================================
static void test_resample_simple()
{
  vcl_cout << "*******************************\n"
           << " Testing vil3d_resample_simple\n"
           << "*******************************\n";

  int sni=3;
  int snj=3;
  int snk=3;
  vil3d_image_view<int> src(sni, snj, snk);
  for (int i=0; i<sni; ++i)
    for (int j=0; j<snj; ++j)
      for (int k=0; k<snk; ++k)
        src(i,j,k) = 10*k;
      //src(i,j,k) = k*snj + j*sni + i;

  int dni=6;
  int dnj=6;
  int dnk=6;
  vil3d_image_view<int> dst(dni, dnj, dnj);

  vil3d_resample_simple(src, dst, 2, 2, 2);

  // Testing
  vil3d_print_all(vcl_cout, src);
  vil3d_print_all(vcl_cout, dst);
  ///

  bool all_voxs_correct = true;
  for (int i=0; i<dni; ++i)
    for (int j=0; j<dnj; ++j)
      for (int k=0; k<dnk; ++k)
        all_voxs_correct = all_voxs_correct && (src(i/2, j/2, k/2)==dst(i,j,k));

  TEST("All voxel values correct", all_voxs_correct, true);
}


//==================================================================================
static void test_resample_trilinear()
{
  vcl_cout << "**********************************\n"
           << " Testing vil3d_resample_trilinear\n"
           << "**********************************\n";

  unsigned sni=3;
  unsigned snj=3;
  unsigned snk=3;
  vil3d_image_view<int> src(sni, snj, snk);
  for (unsigned i=0; i<sni; ++i)
    for (unsigned j=0; j<snj; ++j)
      for (unsigned k=0; k<snk; ++k)
        src(i,j,k) = 10*k + 100*j + 1000*i;

  vil3d_image_view<int> dst;
  vil3d_resample_trilinear(src, dst, 2.0, 2.0, 2.0);
  unsigned dni = dst.ni();
  unsigned dnj = dst.nj();
  unsigned dnk = dst.nk();


  // Testing
  vil3d_print_all(vcl_cout, src);
  vil3d_print_all(vcl_cout, dst);
  ///


  bool all_voxs_correct = true;
  for (unsigned i=0; i+1<dni; ++i)
    for (unsigned j=0; j+1<dnj; ++j)
      for (unsigned k=0; k+1<dnk; ++k)
        all_voxs_correct = all_voxs_correct && dst(i,j,k)==int(5*k + 50*j + 500*i);

  TEST("Voxel values correct", all_voxs_correct, true);
}

//==================================================================================
static void test_resample_trilinear_scale_2()
{
  vcl_cout << "******************************************\n"
           << " Testing vil3d_resample_trilinear_scale_2\n"
           << "******************************************\n";

  unsigned sni=3;
  unsigned snj=3;
  unsigned snk=3;
  vil3d_image_view<int> src(sni, snj, snk);
  for (unsigned i=0; i<sni; ++i)
    for (unsigned j=0; j<snj; ++j)
      for (unsigned k=0; k<snk; ++k)
        src(i,j,k) = 10000 + 10*k + 100*j + 1000*i;

  vil3d_image_view<int> dst;
  vil3d_resample_trilinear_scale_2(src, dst);
  unsigned dni = dst.ni();
  unsigned dnj = dst.nj();
  unsigned dnk = dst.nk();


  // Testing
  vil3d_print_all(vcl_cout, src);
  vil3d_print_all(vcl_cout, dst);
  ///


  bool all_voxs_correct = true;
  for (unsigned i=0; i<dni; ++i)
    for (unsigned j=0; j<dnj; ++j)
      for (unsigned k=0; k<dnk; ++k)
        all_voxs_correct = all_voxs_correct && dst(i,j,k)==int(10000 + 5*k + 50*j + 500*i);

  TEST("Voxel values correct", all_voxs_correct, true);
}


//==================================================================================
//==================================================================================
static void test_resample()
{
  test_resample_simple();
  test_resample_trilinear();
  test_resample_trilinear_scale_2();
}


//==================================================================================
//==================================================================================
TESTMAIN(test_resample);
