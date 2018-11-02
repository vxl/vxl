// This is core/vil/algo/tests/test_algo_abs_shuffle_distance.cxx
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \date   23 Feb 2005

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_abs_shuffle_distance.h>
#include <vil/vil_crop.h>

static void asd_fill_image(vil_image_view<vxl_byte>& image)
{
  for (unsigned j=0;j<image.nj();++j)
    for (unsigned i=0;i<image.ni();++i)
      image(i,j)= vxl_byte(i);
}

static void test_algo_abs_shuffle_distance()
{
  std::cout << "**********************************\n"
           << " Testing vil_abs_shuffle_distance\n"
           << "**********************************\n";

  const unsigned int ni = 30, nj = 40;
  vil_image_view<vxl_byte> image0(ni,nj);

  vil_structuring_element se;
  se.set_to_disk(1.5);
  std::cout<<"Structuring element: "<<se<<std::endl;

  asd_fill_image(image0);

  TEST_NEAR("Shuffle dist to self = 0",
            vil_abs_shuffle_distance(image0,image0,se),0.0,1e-6);

  unsigned ni1=ni-10, nj1=nj-10;
  vil_image_view<vxl_byte> sub_im1 = vil_crop(image0,2,ni1,2,nj1);
  vil_image_view<vxl_byte> sub_im2 = vil_crop(image0,3,ni1,2,nj1);
  vil_image_view<vxl_byte> sub_im3 = vil_crop(image0,4,ni1,2,nj1);

  TEST_NEAR("Shuffle dist to self, 1 pixel displacement",
            vil_abs_shuffle_distance(sub_im1,sub_im2,se,false),0.0,1e-6);

  TEST_NEAR("Shuffle dist to self, 2 pixel displacement",
            vil_abs_shuffle_distance(sub_im1,sub_im3,se,false),1.0,1e-6);

  std::cout<<"Using larger radius: "<<std::endl;
  se.set_to_disk(2.5);
  std::cout<<"Structuring element: "<<se<<std::endl;
  TEST_NEAR("Shuffle dist to self, 2 pixel displacement",
            vil_abs_shuffle_distance(sub_im1,sub_im3,se,false),0.0,1e-6);

#if 0
  // Timing test
  image0.set_size(128,128);
  asd_fill_image(image0);

  std::cout<<"Start."<<std::endl;
  double sum=0.0;
  for (unsigned i=0;i<100;++i)
    sum+=vil_abs_shuffle_distance(image0,image0,se,false);
  std::cout<<"Stop."<<std::endl;
#endif // 0
}

TESTMAIN(test_algo_abs_shuffle_distance);
