// This is mul/ipts/tests/test_scale_space_peaks.cxx

#include "testlib/testlib_test.h"
#include <ipts/ipts_scale_space_peaks.h>
#include "vgl/vgl_point_3d.h"

static void test_scale_space_peaks_float()
{
  // Just check it compiles cleanly first
  vimt_image_2d_of<float> image;
  image.image().set_size(10,10);
  image.image().fill(10);
  image.image()(3,7)=18;  // One peak
  std::vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,image,image,image,float(0),false);
}

static void test_scale_space_peaks()
{
  test_scale_space_peaks_float();
}

TESTMAIN(test_scale_space_peaks);
