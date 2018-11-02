#include <iostream>
#include <string>
#include <vector>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_matrix_fixed.h>

#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_image.h>
#include <ihog/ihog_sample_grid_bilin.h>

#include "../breg3d_homography_generator.h"
#include "../breg3d_lm_direct_homography_generator.h"
#include "../breg3d_gdbicp_homography_generator.h"

static void test_homography_generator()
{
  START("breg3d_homography_generator test");

  std::string root_dir = testlib_root_dir();
  std::string image_file =
    root_dir + "/contrib/gel/mrc/vpgl/ihog/tests/dalmation.tif";

  vil_image_view_base_sptr img0_base = vil_load(image_file.c_str());
  if (!img0_base) {
    std::cerr << "error loading image.\n";
    TEST("FAILED TO LOAD TEST IMAGE",false,true);
    return;
  }
  vil_image_view<vxl_byte> *img0_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img0_base.ptr());
  unsigned ni = img0_byte->ni(), nj = img0_byte->nj();
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(*img0_byte,img0);
  vil_image_view<float> mask0(ni,nj);
  mask0.fill(1.0f);

  // construct arbitrary homography
  vnl_matrix_fixed<double,2,3> HA;
  double rot_angle = 0.1;

  HA(0,0) = std::cos(rot_angle);    HA(0,1) = std::sin(rot_angle);  HA(0,2) =  -20.0;
  HA(1,0) = -std::sin(rot_angle);  HA(1,1) = std::cos(rot_angle) ; HA(1,2) = 30.0;

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

  // debug
  vil_image_view<vxl_byte> byte_image(ni,nj);
  vil_convert_cast(img0,byte_image);
  vil_save(byte_image,"img0.tiff");
  vil_convert_cast(warped_img,byte_image);
  vil_save(byte_image,"img1.tiff");
  vil_save(warped_mask,"mask.tiff");

    // compute homography with gdbicp optimizer
  breg3d_gdbicp_homography_generator db_gen;
  db_gen.set_image0(&img0);
  db_gen.set_image1(&warped_img);
  db_gen.set_mask1(&warped_mask);
  db_gen.set_projective(false);
  ihog_transform_2d xform_out2 = db_gen.compute_homography();

  // compute homography with direct optimizer
  breg3d_lm_direct_homography_generator lm_gen;
  lm_gen.set_image0(&img0);
  lm_gen.set_image1(&warped_img);
  lm_gen.set_mask1(&warped_mask);
  lm_gen.set_projective(false);
  ihog_transform_2d xform_out1 = lm_gen.compute_homography();

  // debug: warp images back to original with computed homographies
  ihog_image<float> sample_im1, sample_im2;

  ihog_image<float> og_img1(img0,xform_out1.inverse());
  ihog_resample_bilin(og_img1,sample_im1,p,u,v,ni,nj);
  vil_image_view<float> warped_img1 = sample_im1.image();

  ihog_image<float> og_img2(img0,xform_out2.inverse());
  ihog_resample_bilin(og_img2,sample_im2,p,u,v,ni,nj);
  vil_image_view<float> warped_img2 = sample_im2.image();

  vil_convert_cast(warped_img1,byte_image);
  vil_save(byte_image,"img1_lm.tiff");
  vil_convert_cast(warped_img2,byte_image);
  vil_save(byte_image,"img1_db.tiff");

  std::cout << "original homography:\n"
           << xform_in.get_matrix() << '\n' << '\n'
           << "lm generated homography:\n"
           << xform_out1.get_matrix() << '\n' << '\n'
           << "db generated homography:\n"
           << xform_out2.get_matrix() << '\n' << std::endl;

  return;
}


TESTMAIN( test_homography_generator );
