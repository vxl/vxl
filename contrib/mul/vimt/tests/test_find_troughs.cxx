// This is mul/vimt/tests/test_find_troughs.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vxl_config.h> // for vxl_byte
#include <vimt/algo/vimt_find_troughs.h>

static void test_find_troughs_byte()
{
  vimt_image_2d_of<vxl_byte> image0;
  image0.image().set_size(10,10);
  image0.image().fill(10);
  image0.image()(3,7)=8;  // One trough
  image0.image()(7,5)=9;  // Another trough

  vcl_vector<vgl_point_2d<unsigned> > im_troughs;
  vimt_find_image_troughs_3x3(im_troughs,image0.image());

  TEST("Number of troughs",im_troughs.size(),2);
  TEST_NEAR("Peak 0",(im_troughs[0]-vgl_point_2d<unsigned>(7,5)).sqr_length(),0,1e-12);
  TEST_NEAR("Peak 1",(im_troughs[1]-vgl_point_2d<unsigned>(3,7)).sqr_length(),0,1e-12);

  vcl_vector<vgl_point_2d<double> > w_troughs;
  vimt_transform_2d w2i;
  w2i.set_translation(2,3);
  image0.set_world2im(w2i);
  vimt_find_world_troughs_3x3(w_troughs,image0);

  TEST("Number of troughs",w_troughs.size(),2);
  TEST_NEAR("Peak 0",(w_troughs[0]-vgl_point_2d<double>(5,2)).sqr_length(),0,1e-12);
  TEST_NEAR("Peak 1",(w_troughs[1]-vgl_point_2d<double>(1,4)).sqr_length(),0,1e-12);
}

static void test_find_troughs()
{
  test_find_troughs_byte();
}

TESTMAIN(test_find_troughs);
