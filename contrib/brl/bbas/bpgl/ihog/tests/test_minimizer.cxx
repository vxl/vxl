#include <iostream>
#include <string>
#include <vector>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_double_2x3.h>

#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_image.h>
#include <ihog/ihog_sample_grid_bilin.h>
#include <ihog/ihog_minimizer.h>

static void test_minimizer()
{
  START("ihog minimizer test");
  std::string root_dir = testlib_root_dir();
  std::string image_file =
    root_dir + "/contrib/brl/bbas/bpgl/ihog/tests/dalmation.tif";
  bool exists = vul_file::exists(image_file);
  vil_image_view_base_sptr img0_base = nullptr;
  if(exists){
    std::cout << "File " << image_file << " exists\n";
    img0_base = vil_load(image_file.c_str());
  }
  if (!img0_base) {
    std::cerr << "error loading image.\n";
    TEST("FAILED TO LOAD TEST IMAGE",false,true);
    return;
  }
  auto *img0_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img0_base.ptr());
  unsigned ni = img0_byte->ni(), nj = img0_byte->nj();
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(*img0_byte,img0);
  vil_image_view<float> mask0(ni,nj);
  mask0.fill(1.0f);

  // construct arbitrary homography
  vnl_double_2x3 HA;
  double rot_angle = 0.15;
  double tx = -20.0, ty = 30.0;
  HA(0,0) = std::cos(rot_angle);   HA(0,1) = std::sin(rot_angle);  HA(0,2) = tx;
  HA(1,0) = -std::sin(rot_angle);  HA(1,1) = std::cos(rot_angle) ; HA(1,2) = ty;

  ihog_transform_2d xform_in;
  xform_in.set_affine(HA);

  // warp image with homography
  ihog_image<float> sample_im;
  ihog_image<float> sample_mask;
  vgl_point_2d<double> p(0,0);
  vgl_vector_2d<double> u(1,0);
  vgl_vector_2d<double> v(0,1);

  ihog_image<float> curr_img(img0,xform_in.inverse());
  ihog_resample_bilin(curr_img,sample_im,p,u,v,ni,nj);
  vil_image_view<float> warped_img = sample_im.image();

  ihog_image<float> curr_mask(mask0,xform_in.inverse());
  ihog_resample_bilin(curr_mask,sample_mask,p,u,v,ni,nj);
  vil_image_view<float> warped_mask = sample_mask.image();
#if 0 //for debug
  vil_image_view<vxl_byte> byte_image(ni,nj);
  vil_convert_cast(img0,byte_image);
  vil_save(byte_image,"img0.tiff");
  vil_convert_cast(warped_img,byte_image);
  vil_save(byte_image,"img1.tiff");
  vil_save(warped_mask,"mask.tiff");
#endif
  int border = 2;
  ihog_world_roi roi(img0.ni()- 2*border,
                     img0.nj()- 2*border,
                     vgl_point_2d<double>(border,border));
  ihog_transform_2d init_xform;
  vnl_double_2x3 init_H(1,0,0, 0,1,0);
  init_xform.set_affine(init_H);

  ihog_image<float> from_img(img0, init_xform);
  ihog_image<float> to_img(warped_img, ihog_transform_2d());
  ihog_image<float> mask_img(warped_mask, ihog_transform_2d());
  ihog_minimizer minimizer(from_img, to_img, mask_img, roi, false);
  minimizer.minimize(init_xform);
  double error = minimizer.get_end_error();
  std::cout << "end_error = " << error << '\n'

           << "original homography:\n"
           << xform_in.get_matrix() << std::endl << '\n'

           << "lm generated homography:\n"
           << init_xform.get_matrix() << std::endl << std::endl;
  //test result
  vgl_point_2d<double> p0 = init_xform.origin();
  vgl_vector_2d<double> du = init_xform.delta(p0, vgl_vector_2d<double>(1,0));
  double ang = std::acos(du.x());
  double err_ang = std::fabs(ang-rot_angle);
  double err_trans = (std::fabs(p0.x()-tx) + std::fabs(p0.y()-ty))/100;
  TEST_NEAR("rigid_body trans",err_ang+err_trans,0.0, 0.01);
#if 0
  std::string dest_file = root_dir + "/contrib/gel/mrc/vpgl/icam/tests/images/calibration/frame_142.png";
  std::string source_file = root_dir + "/contrib/gel/mrc/vpgl/icam/tests/images/calibration/frame_145.png";
  vil_image_view_base_sptr dest_img_base = vil_load(dest_file.c_str());
  if (!dest_img_base) {
    std::cerr << "error loading image.\n";
    return;
  }
  vil_image_view_base_sptr source_img_base = vil_load(source_file.c_str());
  if (!source_img_base) {
    std::cerr << "error loading image.\n";
    return;
  }
    vil_image_view<vxl_byte> *dest_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dest_img_base.ptr());
  vil_image_view<vxl_byte> *source_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(source_img_base.ptr());
  unsigned ni_d = dest_img_byte->ni(), nj_d = dest_img_byte->nj();
  vil_image_view<float> dest_img_flt(ni_d,nj_d);
  vil_convert_cast(*dest_img_byte,dest_img_flt);
  vil_image_view<float> source_img_flt(ni,nj);
  vil_convert_cast(*source_img_byte,source_img_flt);

  ihog_world_roi roid(ni_d - 2*border,
                      nj_d - 2*border,
                      vgl_point_2d<double>(border,border));

  ihog_transform_2d init_xform_d;
  init_xform_d.set_affine(init_H);
  ihog_image<float> from_img_d(source_img_flt, init_xform_d);
  ihog_image<float> to_img_d(dest_img_flt, ihog_transform_2d());
  ihog_minimizer minimizer_d(from_img_d, to_img_d, roid);
  minimizer_d.minimize(init_xform_d);
  double error_d = minimizer_d.get_end_error();
  std::cout << "end_error = " << error_d << '\n'
           << "lm generated homography downtown:\n"
           << init_xform_d.get_matrix() << std::endl << std::endl;
  // create mapped image
  ihog_image<float> mapped_source;
  ihog_resample_bilin(from_img_d, mapped_source, init_xform_d.inverse());
  vil_save(mapped_source.image(), (root_dir + "/contrib/gel/mrc/vpgl/icam/tests/images/calibration/hog_mapped_f145.tif").c_str());
#endif
}


TESTMAIN( test_minimizer );
