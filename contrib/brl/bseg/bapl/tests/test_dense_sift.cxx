#include <iostream>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_dense_sift_sptr.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>

#include<vnl/vnl_math.h>

static void test_dense_sift()
{
  unsigned ni = 10, nj = 10, ki = ni / 2, kj = nj / 2;
  vil_image_view<vxl_byte> img(ni,nj,1);

  for(unsigned j = 0; j < nj; ++j)
    for(unsigned i = 0; i < ni; ++i)
      if(i >= ki)
        img(i,j,0) = 255;
      else
        img(i,j,0) = 0;

  bapl_dense_sift_sptr dense_sift_sptr =
    new bapl_dense_sift(vil_new_image_resource_of_view(img), 2, 0);

  bapl_lowe_keypoint_sptr keypoint;

  dense_sift_sptr->make_keypoint(keypoint, ki, kj);

  //orientation should be 90 degrees
  TEST_NEAR("Testing orientation.", keypoint->orientation(), vnl_math::pi, 1e-3);

  return;
}

TESTMAIN(test_dense_sift);
