// This is mul/vimt/tests/test_find_peaks.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vxl_config.h> // for vxl_byte
#include <vimt/algo/vimt_find_peaks.h>

void test_find_peaks_byte()
{
  vimt_image_2d_of<vxl_byte> image0;
  image0.image().set_size(10,10);
  image0.image().fill(10);
  image0.image()(3,7)=18;  // One peak
  image0.image()(7,5)=19;  // Another peak

  vcl_vector<vgl_point_2d<unsigned> > im_peaks;
  vcl_vector<vxl_byte> peak_values;
  vimt_find_image_peaks(im_peaks, peak_values, image0.image(), vxl_byte(12), 2);

  TEST("Number of peaks",im_peaks.size(),2);
  TEST_NEAR("Peak 0",(im_peaks[0]-vgl_point_2d<unsigned>(7,5)).sqr_length(),0,1e-6);
  TEST_NEAR("Peak 1",(im_peaks[1]-vgl_point_2d<unsigned>(3,7)).sqr_length(),0,1e-6);

  vimt_find_image_peaks_3x3(im_peaks,peak_values,image0.image());

  TEST("Number of peaks",im_peaks.size(),2);
  TEST_NEAR("Peak 0",(im_peaks[0]-vgl_point_2d<unsigned>(7,5)).sqr_length(),0,1e-6);
  TEST("Peak 0 value",peak_values[0],19);
  TEST_NEAR("Peak 1",(im_peaks[1]-vgl_point_2d<unsigned>(3,7)).sqr_length(),0,1e-6);
  TEST("Peak 1 value",peak_values[1],18);

  vcl_vector<vgl_point_2d<double> > w_peaks;
  vimt_transform_2d w2i;
  w2i.set_translation(2,3);
  image0.set_world2im(w2i);
  vimt_find_world_peaks_3x3(w_peaks,image0);

  TEST("Number of peaks",w_peaks.size(),2);
  TEST_NEAR("Peak 0",(w_peaks[0]-vgl_point_2d<double>(5,2)).sqr_length(),0,1e-6);
  TEST_NEAR("Peak 1",(w_peaks[1]-vgl_point_2d<double>(1,4)).sqr_length(),0,1e-6);

  vgl_point_2d<unsigned> im_p1 = vimt_find_max(image0.image());
  TEST_NEAR("Global peak",(im_p1-vgl_point_2d<unsigned>(7,5)).sqr_length(),0,1e-6);

  vgl_point_2d<double> w_p1 = vimt_find_max(image0);
  TEST_NEAR("Global peak",(w_p1-vgl_point_2d<double>(5,2)).sqr_length(),0,1e-6);
}

MAIN( test_find_peaks )
{
  START( "vimt_find_peaks" );

  test_find_peaks_byte();

  SUMMARY();
}
