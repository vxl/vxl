// This is mul/ipts/tests/test_scale_space_peaks.h

#include <testlib/testlib_test.h>

#include <ipts/ipts_scale_space_peaks.h>

void test_scale_space_peaks_float()
{
  // Just check it compiles cleanly first
  vimt_image_2d_of<float> image;
  vcl_vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,image,image,image,false);
}

MAIN( test_scale_space_peaks )
{
  START( "ipts_scale_space_peaks" );
  test_scale_space_peaks_float();

  SUMMARY();
}
