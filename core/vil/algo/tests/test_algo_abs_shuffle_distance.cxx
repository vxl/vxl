// This is core/vil/algo/tests/test_algo_abs_shuffle_distance.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \date   23 Feb 2005

#include <vcl_iostream.h>
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
  vcl_cout << "*********************************\n"
           << " Testing vil_abs_shuffle_distance\n"
           << "*********************************\n";

  const unsigned int ni = 30, nj = 40;
  vil_image_view<vxl_byte> image0(ni,nj);

  vil_structuring_element se;
  se.set_to_disk(1.5);
  vcl_cout<<"Structuring element: "<<se<<vcl_endl;

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
            
  vcl_cout<<"Using larger radius: "<<vcl_endl;
  se.set_to_disk(2.5);
  vcl_cout<<"Structuring element: "<<se<<vcl_endl;
  TEST_NEAR("Shuffle dist to self, 2 pixel displacement",
            vil_abs_shuffle_distance(sub_im1,sub_im3,se,false),0.0,1e-6);
            
/*
  // Timing test
  image0.set_size(128,128);
  asd_fill_image(image0);

  vcl_cout<<"Start."<<vcl_endl;
  double sum=0.0;
  for (unsigned i=0;i<100;++i)
    sum+=vil_abs_shuffle_distance(image0,image0,se,false);
  vcl_cout<<"Stop."<<vcl_endl;
*/
}

TESTMAIN(test_algo_abs_shuffle_distance);
