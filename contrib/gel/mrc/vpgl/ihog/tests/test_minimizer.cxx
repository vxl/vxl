#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_image.h>
#include <ihog/ihog_sample_grid_bilin.h>
#include <ihog/ihog_minimizer.h>

static void test_minimizer()
{
  START("ihog minimizer test");
  vcl_string root_dir = testlib_root_dir();
  vcl_string image_file =
    root_dir + "/contrib/gel/mrc/vpgl/ihog/tests/dalmation.tif";
  vil_image_view_base_sptr img0_base = vil_load(image_file.c_str());
  if (!img0_base) {
    vcl_cerr << "error loading image." << vcl_endl;
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
  double rot_angle = 0.15;
  double tx = -20.0, ty = 30.0;
  HA(0,0) = vcl_cos(rot_angle);   HA(0,1) = vcl_sin(rot_angle);  HA(0,2) = tx;
  HA(1,0) = -vcl_sin(rot_angle);  HA(1,1) = vcl_cos(rot_angle) ; HA(1,2) = ty;

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
  vnl_matrix<double> init_H(3,3);
  init_H.set_identity();
  init_xform.set_affine(init_H.extract(2,3,0,0));

  ihog_image<float> from_img(img0, init_xform);
  ihog_image<float> to_img(warped_img, ihog_transform_2d());
  ihog_image<float> mask_img(warped_mask, ihog_transform_2d()); 
  ihog_minimizer minimizer(from_img, to_img, mask_img, roi, false);
  minimizer.minimize(init_xform);
  double error = minimizer.get_end_error();
  vcl_cout << "end_error = " << error << '\n';

  vcl_cout << "original homography: " << vcl_endl;
  vcl_cout << xform_in.matrix() << vcl_endl << vcl_endl;

  vcl_cout << "lm generated homography: " << vcl_endl;
  vcl_cout << init_xform.matrix() << vcl_endl << vcl_endl;
  //test result
  vgl_point_2d<double> p0 = init_xform.origin();
  vgl_vector_2d<double> du = init_xform.delta(p0, vgl_vector_2d<double>(1,0));
  double ang = vcl_acos(du.x());
  double err_ang = vcl_fabs(ang-rot_angle);
  double err_trans = (vcl_fabs(p0.x()-tx) + vcl_fabs(p0.y()-ty))/100;
  TEST_NEAR("rigid_body trans",err_ang+err_trans,0.0, 0.01);
}



TESTMAIN( test_minimizer );
