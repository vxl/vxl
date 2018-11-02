#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_poly_radial_distortion.h>
#include <vpgl/algo/vpgl_lens_warp_mapper.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_bilin_interp.h>
#include <vil/vil_math.h>
#include <vul/vul_timer.h>

static
vil_image_view<vxl_byte>
make_checker_board(unsigned int ni, unsigned int nj, unsigned int s)
{
  vil_image_view<vxl_byte> cb(ni,nj,1);
  for (unsigned int i=0; i<ni; ++i)
    for (unsigned int j=0; j<nj; ++j)
      if ((i/s + j/s)%2 == 0)
        cb(i,j) = 0;
      else
        cb(i,j) = 255;
  return cb;
}

static
vxl_byte
interpolator(vil_image_view<vxl_byte> const& view,
             double x, double y, unsigned p)
{
  return static_cast<vxl_byte>(vil_bilin_interp_safe(view, x, y, p));
}


static void test_lens_warp_mapper()
{
  {
    unsigned int img_size = 128;
    vgl_point_2d<double> center(img_size/2.0, img_size/2.0);
    double k[] = { 0.001, 0.0001};
    vpgl_poly_radial_distortion<double,2> rd(center, k);

    // make the checkerboard image
    vil_image_view<vxl_byte> cb = make_checker_board(img_size,img_size,8);
    vil_save(cb,"cb.jpg");
    std::cout << "<DartMeasurementFile name=\"Checker Board\" type=\"image/jpeg\"> "
             << "cb.jpg </DartMeasurementFile>" <<std::endl;

    vil_image_view<vxl_byte> unwarp_cb(cb.ni(),cb.nj(),1);
    vpgl_lens_unwarp(cb,unwarp_cb,rd,interpolator);
    vil_save(unwarp_cb,"unwarp_cb.jpg");
    std::cout << "<DartMeasurementFile name=\"Unwarped Checker Board\" type=\"image/jpeg\"> "
             << "unwarp_cb.jpg </DartMeasurementFile>" <<std::endl;

    vil_image_view<vxl_byte> warp_cb(cb.ni(),cb.nj(),1);
    vpgl_lens_warp(unwarp_cb,warp_cb,rd,interpolator);
    vil_save(warp_cb,"warp_cb.jpg");
    std::cout << "<DartMeasurementFile name=\"Rewarped Checker Board\" type=\"image/jpeg\"> "
             << "warp_cb.jpg </DartMeasurementFile>" <<std::endl;

    vil_image_view<vxl_byte> warp2_cb = vpgl_lens_warp_resize(cb,vxl_byte(),rd,interpolator);
    vil_save(warp2_cb,"warp2_cb.jpg");
    std::cout << "<DartMeasurementFile name=\"Lens Distorted Checker Board (resized)\" type=\"image/jpeg\"> "
             << "warp2_cb.jpg </DartMeasurementFile>" <<std::endl;

    vil_image_view<vxl_byte> unwarp2_cb = vpgl_lens_unwarp_resize(warp2_cb,vxl_byte(),rd,interpolator);
    vil_save(unwarp2_cb,"unwarp2_cb.jpg");
    std::cout << "<DartMeasurementFile name=\"Undistorted Checker Board (resized)\" type=\"image/jpeg\"> "
             << "unwarp2_cb.jpg </DartMeasurementFile>" <<std::endl;

    //TEST("warped bounds", distort2.width() == distort2.height() && distort2.width() == 99, true);
    //TEST_NEAR("warped offset x", distort2.offset_x(), 14.5431, 1e-3);
    //TEST_NEAR("warped offset y", distort2.offset_y(), 14.5431, 1e-3);
    double rms = std::sqrt(vil_math_ssd(cb,warp_cb,double())/(cb.ni()*cb.nj()));
    // This is sort of an arbitrary test
    TEST("compare unwarped to original", rms < 50, true);
    std::cout << "image rms error: " << rms << std::endl;
  }
  //===================================================
  // timing tests
  {
    vgl_point_2d<double> center(256.0, 256.0);
    double k[2] = { 0.001, 0.0001};
    vpgl_poly_radial_distortion<double,2> rd(center, k);

    vil_image_view<vxl_byte> cb = make_checker_board(512,512,32);

    vil_image_view<vxl_byte> warp_cb(cb.ni(),cb.nj(),1);
    vul_timer t;
    vpgl_lens_warp(cb,warp_cb,rd,interpolator);
    long time = t.user();
    std::cout << "warp: time = "<<time/1000.0<<std::endl;

    vil_image_view<vxl_byte> unwarp_cb(cb.ni(),cb.nj(),1);
    t.mark();
    vpgl_lens_unwarp(warp_cb,unwarp_cb,rd,interpolator);
    time = t.user();
    std::cout << "unwarp: time = "<<time/1000.0<<std::endl;
  }
}


TESTMAIN(test_lens_warp_mapper);
