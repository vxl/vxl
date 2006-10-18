// This is mul/vil3d/tests/test_algo_corners.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil3d/algo/vil3d_corners.h>
#include <vil3d/algo/vil3d_find_peaks.h>
#include <vil3d/vil3d_print.h>

static void test_algo_corners_byte()
{
  vcl_cout << "************************\n"
           << " Testing vil3d_corners\n"
           << "************************\n";

  // Create a cube in a blank image
  vil3d_image_view<vxl_byte> image(16,16,16);
  image.fill(vxl_byte(0));
  for (unsigned k=5;k<=12;++k)
    for (unsigned j=5;j<=12;++j)
      for (unsigned i=5;i<=12;++i)
        image(i,j,k)=100;

  vil3d_image_view<float> corner_im;

  vil3d_corners<vxl_byte,float> corners;
  corners.cornerness1(image,corner_im);

  const vcl_ptrdiff_t istep = corner_im.istep(),
                      jstep=corner_im.jstep(),
                      kstep=corner_im.kstep();

  // Due to smoothing operations, corners just inside true cube corners
  TEST("Corner at (6,6,6)",vil3d_is_peak26(&corner_im(6,6,6),istep,jstep,kstep),true);
  TEST("Corner at (11,11,11)",vil3d_is_peak26(&corner_im(11,11,11),istep,jstep,kstep),true);
  TEST("Corner at (6,6,11)",vil3d_is_peak26(&corner_im(6,6,11),istep,jstep,kstep),true);
  TEST("Corner at (6,11,11)",vil3d_is_peak26(&corner_im(6,11,11),istep,jstep,kstep),true);

  vcl_vector<vgl_point_3d<int> > peaks;
  float min_thresh = 0.0;
  vil3d_find_peaks_26(peaks,corner_im,min_thresh,true);
  vcl_cout<<"Peaks: "<<vcl_endl;
  for (unsigned i=0;i<peaks.size();++i)
    vcl_cout<<i<<") "<<peaks[i]<<vcl_endl;
}

static void test_algo_corners()
{
  test_algo_corners_byte();
}

TESTMAIN(test_algo_corners);
