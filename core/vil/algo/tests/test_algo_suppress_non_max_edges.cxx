// This is core/vil/algo/tests/test_algo_suppress_non_max_edges.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \date   15 June 2005
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_suppress_non_max_edges.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_print.h>

static void test_algo_suppress_non_max_edges()
{
  vcl_cout << "*******************\n"
           << " Testing vil_suppress_non_max_edges\n"
           << "*******************\n";

  unsigned ni=20,nj=16;
  vil_image_view<vxl_byte> image0; image0.set_size(ni,nj);

  // Create white square on a slowly varying background
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i) image0(i,j)=i+j;

  for (unsigned j=5;j<10;++j)
    for (unsigned i=5;i<10;++i) image0(i,j)=250;

  vil_image_view<float> gradi, gradj, grad_mag;
  vil_sobel_3x3(image0, gradi, gradj);
  vil_suppress_non_max_edges(gradi, gradj, 10.0, grad_mag);

  TEST("Horizontal edge is thin (1)",grad_mag(3,8),0.0);
  TEST("Horizontal edge is thin (2)",grad_mag(4,8)>0,true);
  TEST("Horizontal edge is thin (3)",grad_mag(5,8),0.0);

  TEST("Vertical edge is thin (1)",grad_mag(8,3),0.0);
  TEST("Vertical edge is thin (2)",grad_mag(8,4)>0,true);
  TEST("Vertical edge is thin (3)",grad_mag(8,5),0.0);

  TEST("Corner eroded by smoothing",grad_mag(4,4),0);
}

TESTMAIN(test_algo_suppress_non_max_edges);
