#include <testlib/testlib_test.h>

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
#include <ihog/ihog_minfo_cost_func.h>

#include <testlib/testlib_root_dir.h>

void test_good_image() {

  vcl_string root_dir = testlib_root_dir();
  vcl_string image_file =
    root_dir + "/contrib/gel/mrc/vpgl/ihog/tests/dalmation.tif";
  vil_image_view_base_sptr img0_base = vil_load(image_file.c_str());
  if (!img0_base) {
    vcl_cerr << "error loading image.\n";
    TEST("FAILED TO LOAD TEST IMAGE",false,true);
    return;
  }
  //vil_image_view<vxl_byte> img_0(img0_base);

  vil_image_view<vxl_byte> img_0 = vil_load("H:/projects/MultiScale/view_07_cropped.png");
  //vil_image_view<vxl_byte> img_1 = vil_load("H:/projects/MultiScale/img1.tiff");

  unsigned ni = img_0.ni(), nj = img_0.nj();
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(img_0,img0);
  vil_image_view<float> mask0(ni,nj);
  mask0.fill(1.0f);

  /*
  ni = img_1.ni(); nj = img_1.nj();
  vil_image_view<float> warped_img(ni,nj);
  vil_convert_cast(img_1, warped_img);
  */

  // construct arbitrary homography
  double tx = -20.3, ty = 30.7;

  ihog_transform_2d xform_in;
  xform_in.set_translation_only(tx, ty);

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
#if 1 //for debug
  vil_image_view<vxl_byte> byte_image(ni,nj);
  vil_convert_cast(img0,byte_image);
  vil_save(byte_image,"img0.tiff");
  vil_convert_cast(warped_img,byte_image);
  vil_save(byte_image,"img1.tiff");
#endif
  
  
  int border = 2;
  ihog_world_roi roi(img0.ni()- 2*border,
                     img0.nj()- 2*border,
                     vgl_point_2d<double>(border,border));
  ihog_transform_2d init_xform;
  init_xform.set_translation_only(0,0);
  
  ihog_image<float> from_img(img0, init_xform);
  ihog_image<float> to_img(warped_img, ihog_transform_2d());
  ihog_image<float> mask_img(warped_mask, ihog_transform_2d());
  ihog_minimizer minimizer(from_img, to_img, mask_img, roi, false);
  minimizer.minimize_using_minfo(init_xform);
  double error = minimizer.get_end_error();
  vcl_cout << "end_error = " << error << '\n'

           << "original homography:\n"
           << xform_in.get_matrix() << vcl_endl << '\n'

           << "lm generated homography:\n"
           << init_xform.get_matrix() << vcl_endl << vcl_endl;
  //test result
  vgl_point_2d<double> p0 = init_xform.origin();
  double err_trans = (vcl_fabs(p0.x()-tx) + vcl_fabs(p0.y()-ty))/100;
  TEST_NEAR("rigid_body trans",err_trans,0.0, 0.01);
}
void scale_im(vil_image_view<float>& im, vil_image_view<vxl_byte>& im_byte, float scale) {
  unsigned ni = im.ni(), nj = im.nj();
  //: scale the image to byte
  for(unsigned j = 0; j<nj; ++j) 
    for(unsigned i = 0; i<ni; ++i) {
        float v = scale*im(i,j);
        if(v>255.0f) v= 255.0f; if(v<0.0f) v = 0.0f;
        im(i,j) = v;
      }
  
  vil_convert_cast(im, im_byte);
}
void test_exp_image() {
  vil_image_view<float> img_0 = vil_load("H:/projects/MultiScale/Baghdad/dim_2/exp_frame_84_ill_bin_2.tiff");
  unsigned ni = img_0.ni(), nj = img_0.nj();
  vil_image_view<vxl_byte> img_0_byte(ni, nj);
  scale_im(img_0, img_0_byte, 255.0f);

  vil_image_view<vxl_byte> img_1 = vil_load("H:/projects/MultiScale/Baghdad/dim_2/cropped_imgs/view_00_cropped_normalized_resampled.png");
  
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(img_0_byte,img0);
  
  vil_image_view<float> img1(img_1.ni(),img_1.nj());
  vil_convert_cast(img_1, img1);
  vil_image_view<float> mask1(img_1.ni(),img_1.nj());
  mask1.fill(1.0f);

  // ------warp img1--------------
  // construct arbitrary homography
  double tx = -20.3, ty = 30.7;
  ihog_transform_2d xform_in;
  xform_in.set_translation_only(tx, ty);
  // warp image with homography
  ihog_image<float> sample_im;
  ihog_image<float> sample_mask;
  vgl_point_2d<double> p(0,0);
  vgl_vector_2d<double> u(1,0);
  vgl_vector_2d<double> v(0,1);
  ihog_image<float> curr_img(img1,xform_in);
  ihog_resample_bilin(curr_img,sample_im,p,u,v,ni,nj);
  vil_image_view<float> warped_img = sample_im.image();
  ihog_image<float> curr_mask(mask1,xform_in);
  ihog_resample_bilin(curr_mask,sample_mask,p,u,v,ni,nj);
  vil_image_view<float> warped_mask = sample_mask.image();
  // ------------------------------

  int border = 2;
  ihog_world_roi roi(img0.ni()- 2*border,
                     img0.nj()- 2*border,
                     vgl_point_2d<double>(border,border));
  ihog_transform_2d init_xform;
  init_xform.set_translation_only(0,0);
  
  ihog_image<float> from_img(img0, init_xform);
  //ihog_image<float> to_img(img1, ihog_transform_2d());
  ihog_image<float> to_img(warped_img, ihog_transform_2d());
  //ihog_image<float> mask_img(mask1, ihog_transform_2d());
  ihog_image<float> mask_img(warped_mask, ihog_transform_2d());
  ihog_minimizer minimizer(from_img, to_img, mask_img, roi, false);
  minimizer.minimize_using_minfo(init_xform);
  double error = minimizer.get_end_error();
  vcl_cout << "end_error = " << error << '\n'

           << "lm generated homography:\n"
           << init_xform.get_matrix() << vcl_endl << vcl_endl;
  //test result
  vgl_point_2d<double> p0 = init_xform.origin();
  vcl_cout << "tx: " << p0.x() << " " << " ty: " << p0.y() << "\n";
 
}

void test_pixelwise_minfo()
{
  //vil_image_view<float> img_0 = vil_load("H:/projects/MultiScale/Baghdad/level_3/exp_frame_84_ill_bin_2.tiff");
  //vil_image_view<float> img_0_o = vil_load("H:/projects/MultiScale/Baghdad/dim_8/exp_frame_84_ill_bin_2.tiff");
  vil_image_view<float> img_0_o = vil_load("H:/projects/MultiScale/Baghdad/dim_4_pert/exp_frame_84_ill_bin_2.tiff");
  vil_image_view<vxl_byte> img_1 = vil_load("H:/projects/MultiScale/Baghdad/level_3/cropped_imgs/view_00_cropped_normalized_resampled.png");

  vil_image_view<float> img_0(img_1.ni(), img_1.nj());
  vil_resample_bilin(img_0_o, img_0, img_1.ni(), img_1.nj());

  unsigned ni = img_0.ni(), nj = img_0.nj();
  vil_image_view<vxl_byte> img_0_byte(ni, nj);
  scale_im(img_0, img_0_byte, 255.0f);
  
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(img_0_byte,img0);
  
  vil_image_view<float> img1(img_1.ni(),img_1.nj());
  vil_convert_cast(img_1, img1);

  vil_image_view<float> mask(ni,nj);
  mask.fill(1.0f);

  // ------warp img1--------------
  // construct arbitrary homography
  double tx = 15.4, ty = 10.9;
  ihog_transform_2d xform_in;
  xform_in.set_translation_only(tx, ty);
  // warp image with homography
  ihog_image<float> sample_im;
  ihog_image<float> sample_mask;
  vgl_point_2d<double> p(0,0);
  vgl_vector_2d<double> u(1,0);
  vgl_vector_2d<double> v(0,1);
  ihog_image<float> curr_img(img1,xform_in);
  ihog_resample_bilin(curr_img,sample_im,p,u,v,ni,nj);
  vil_image_view<float> warped_img_o = sample_im.image();
  ihog_image<float> curr_mask(mask,xform_in);
  ihog_resample_bilin(curr_mask,sample_mask,p,u,v,ni,nj);
  vil_image_view<float> warped_mask_o = sample_mask.image();
  // ------------------------------

  int border = 15;
  ihog_world_roi roi(ni- 2*border, nj- 2*border, vgl_point_2d<double>(border,border));

  ihog_image<float> im0(img0); 
  vnl_vector<double> from_samples = roi.sample(im0);
  ihog_image<float> imask(mask);
  vnl_vector<double> mask_samples = roi.sample(imask);

  //vgl_point_2d<double> p(0,0);
  //vgl_vector_2d<double> u(1,0);
  //vgl_vector_2d<double> v(0,1);

  vil_image_view<float> out(ni, nj);
  out.fill(0.0f);
  int cent_i = ni/2, cent_j = nj/2;

  int rad = 20;
  double mi_min = 10000.0; int tx_min, ty_min;
  vil_image_view<float> out_min;
  for (int tx = -rad; tx < rad+1; tx++) {
    for (int ty = -rad; ty < rad+1; ty++) {
      ihog_transform_2d xform;
      xform.set_translation_only(tx,ty);

      ihog_image<float> sample_im; ihog_image<float> sample_mask;
      
      //ihog_image<float> curr_img(img1,xform);
      ihog_image<float> curr_img(warped_img_o,xform);
      ihog_resample_bilin(curr_img,sample_im,p,u,v,(int)ni,(int)nj);
      vil_image_view<float> warped_img = sample_im.image();
      ihog_image<float> warped_img_i(warped_img);

      //ihog_image<float> curr_mask(mask,xform);
      ihog_image<float> curr_mask(warped_mask_o,xform);
      ihog_resample_bilin(curr_mask,sample_mask,p,u,v,ni,nj);
      vil_image_view<float> warped_mask = sample_mask.image();
      ihog_image<float> warped_mask_i(warped_mask);

      //vcl_stringstream ss; ss << "img1_" << tx << "_" << ty << ".png";
      //vil_image_view<vxl_byte> im_byte(ni, nj);
      //scale_im(warped_img, im_byte, 1.0f);
      //vil_save(im_byte, ss.str().c_str());
      vnl_vector<double> to_samples = roi.sample(warped_img_i);
      vnl_vector<double> to_mask_samples = roi.sample(warped_mask_i);
      //double mi = ihog_minfo_cost_func::entropy_diff(mask_samples, from_samples, to_samples, 16);
      double mi = ihog_minfo_cost_func::entropy_diff(to_mask_samples, from_samples, to_samples, 16);
      if (mi < mi_min) {
        mi_min = mi;
        out_min = warped_img; tx_min = tx; ty_min = ty;
      }
      vcl_cout << "tx: " << tx << " ty: " << ty << " mi: " << mi << vcl_endl;
      out(cent_i + tx, cent_j + ty) = (float)mi;
    }
  }
  vil_save(out, "out_img.tiff");
  float min_value, max_value;
  vil_math_value_range(out, min_value, max_value);
  vil_image_view<vxl_byte> out_byte(ni, nj);
  scale_im(out, out_byte, 255.0f/max_value);
  vil_save(out_byte, "out_img.png");

  vil_image_view<vxl_byte> out_min_byte(ni, nj);
  scale_im(out_min, out_min_byte, 1.0f);
  vcl_stringstream ss; ss << "out_min_img_" << tx_min << "_" << ty_min << ".png";
  vil_save(out_min_byte, ss.str().c_str());

  //: now refine using powell
  ihog_transform_2d init_xform;
  init_xform.set_translation_only(tx_min,ty_min);
  vcl_cout << "applying powell:, init_x: " << tx_min << " init_y: " << ty_min << vcl_endl;

  ihog_image<float> from_img(img0, init_xform);
  ihog_image<float> to_img(img1, ihog_transform_2d());
  ihog_image<float> mask_img(mask, ihog_transform_2d());
  ihog_minimizer minimizer(from_img, to_img, mask_img, roi, false);
  minimizer.minimize_using_minfo(init_xform);
  double error = minimizer.get_end_error();
  vcl_cout << "end_error = " << error << '\n'

           << "lm generated homography:\n"
           << init_xform.get_matrix() << vcl_endl << vcl_endl;
  //test result
  vgl_point_2d<double> p0 = init_xform.origin();
  vcl_cout << "after powell:, x: " << p0.x() << " y: " << p0.y() << vcl_endl;
  
}

void test_exhaustive_minfo()
{
  //vil_image_view<float> img_0 = vil_load("H:/projects/MultiScale/Baghdad/level_3/exp_frame_84_ill_bin_2.tiff");
  //vil_image_view<float> img_0_o = vil_load("H:/projects/MultiScale/Baghdad/dim_8/exp_frame_84_ill_bin_2.tiff");
  vil_image_view<float> img_0_o = vil_load("H:/projects/MultiScale/Baghdad/dim_4_pert/exp_frame_84_ill_bin_2.tiff");
  //vil_image_view<vxl_byte> img_1 = vil_load("H:/projects/MultiScale/Baghdad/level_3/cropped_imgs/view_00_cropped_normalized_resampled.png");
  vil_image_view<vxl_byte> img_1 = vil_load("H:/projects/MultiScale/Baghdad/perturbed_cameras_50/cropped_imgs/view_10_cropped_normalized.png");

  vil_image_view<float> img_0(img_1.ni(), img_1.nj());
  vil_resample_bilin(img_0_o, img_0, img_1.ni(), img_1.nj());

  unsigned ni = img_0.ni(), nj = img_0.nj();
  vil_image_view<vxl_byte> img_0_byte(ni, nj);
  scale_im(img_0, img_0_byte, 255.0f);
  
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(img_0_byte,img0);
  
  vil_image_view<float> img1(img_1.ni(),img_1.nj());
  vil_convert_cast(img_1, img1);

  vil_image_view<float> mask(ni,nj);
  mask.fill(1.0f);

  // ------warp img1--------------
  // construct arbitrary homography
  double tx = 15.4, ty = 10.9;
  ihog_transform_2d xform_in;
  xform_in.set_translation_only(tx, ty);
  // warp image with homography
  ihog_image<float> sample_im;
  ihog_image<float> sample_mask;
  vgl_point_2d<double> p(0,0);
  vgl_vector_2d<double> u(1,0);
  vgl_vector_2d<double> v(0,1);
  ihog_image<float> curr_img(img1,xform_in);
  ihog_resample_bilin(curr_img,sample_im,p,u,v,ni,nj);
  vil_image_view<float> warped_img_o = sample_im.image();
  ihog_image<float> curr_mask(mask,xform_in);
  ihog_resample_bilin(curr_mask,sample_mask,p,u,v,ni,nj);
  vil_image_view<float> warped_mask_o = sample_mask.image();
  // ------------------------------

  int border = 15;
  ihog_world_roi roi(ni- 2*border, nj- 2*border, vgl_point_2d<double>(border,border));

  ihog_image<float> im0(img0); 
  vnl_vector<double> from_samples = roi.sample(im0);
  ihog_image<float> imask(mask);
  vnl_vector<double> mask_samples = roi.sample(imask);

  //vgl_point_2d<double> p(0,0);
  //vgl_vector_2d<double> u(1,0);
  //vgl_vector_2d<double> v(0,1);

  vil_image_view<float> out(ni, nj);
  out.fill(0.0f);
  int cent_i = ni/2, cent_j = nj/2;

  int rad = 20;
  double mi_min = 10000.0; int tx_min, ty_min;
  vil_image_view<float> out_min;
  for (int tx = -rad; tx < rad+1; tx++) {
    for (int ty = -rad; ty < rad+1; ty++) {
      ihog_transform_2d xform;
      xform.set_translation_only(tx,ty);

      ihog_image<float> sample_im; ihog_image<float> sample_mask;
      
      //ihog_image<float> curr_img(img1,xform);
      ihog_image<float> curr_img(warped_img_o,xform);
      ihog_resample_bilin(curr_img,sample_im,p,u,v,(int)ni,(int)nj);
      vil_image_view<float> warped_img = sample_im.image();
      ihog_image<float> warped_img_i(warped_img);

      //ihog_image<float> curr_mask(mask,xform);
      ihog_image<float> curr_mask(warped_mask_o,xform);
      ihog_resample_bilin(curr_mask,sample_mask,p,u,v,ni,nj);
      vil_image_view<float> warped_mask = sample_mask.image();
      ihog_image<float> warped_mask_i(warped_mask);

      //vcl_stringstream ss; ss << "img1_" << tx << "_" << ty << ".png";
      //vil_image_view<vxl_byte> im_byte(ni, nj);
      //scale_im(warped_img, im_byte, 1.0f);
      //vil_save(im_byte, ss.str().c_str());
      vnl_vector<double> to_samples = roi.sample(warped_img_i);
      vnl_vector<double> to_mask_samples = roi.sample(warped_mask_i);
      //double mi = ihog_minfo_cost_func::entropy_diff(mask_samples, from_samples, to_samples, 16);
      double mi = ihog_minfo_cost_func::entropy_diff(to_mask_samples, from_samples, to_samples, 16);
      if (mi < mi_min) {
        mi_min = mi;
        out_min = warped_img; tx_min = tx; ty_min = ty;
      }
      vcl_cout << "tx: " << tx << " ty: " << ty << " mi: " << mi << vcl_endl;
      out(cent_i + tx, cent_j + ty) = (float)mi;
    }
  }
  vil_save(out, "out_img.tiff");
  float min_value, max_value;
  vil_math_value_range(out, min_value, max_value);
  vil_image_view<vxl_byte> out_byte(ni, nj);
  scale_im(out, out_byte, 255.0f/max_value);
  vil_save(out_byte, "out_img.png");

  vil_image_view<vxl_byte> out_min_byte(ni, nj);
  scale_im(out_min, out_min_byte, 1.0f);
  vcl_stringstream ss; ss << "out_min_img_" << tx_min << "_" << ty_min << ".png";
  vil_save(out_min_byte, ss.str().c_str());

  //: now refine using powell
  ihog_transform_2d init_xform;
  init_xform.set_translation_only(tx_min,ty_min);
  vcl_cout << "applying powell:, init_x: " << tx_min << " init_y: " << ty_min << vcl_endl;

  ihog_image<float> from_img(img0, init_xform);
  ihog_image<float> to_img(img1, ihog_transform_2d());
  ihog_image<float> mask_img(mask, ihog_transform_2d());
  ihog_minimizer minimizer(from_img, to_img, mask_img, roi, false);
  minimizer.minimize_using_minfo(init_xform);
  double error = minimizer.get_end_error();
  vcl_cout << "end_error = " << error << '\n'

           << "lm generated homography:\n"
           << init_xform.get_matrix() << vcl_endl << vcl_endl;
  //test result
  vgl_point_2d<double> p0 = init_xform.origin();
  vcl_cout << "after powell:, x: " << p0.x() << " y: " << p0.y() << vcl_endl;
}

static void test_matcher_translation()
{
  
  START ("test matcher translation");

  //test_good_image();
  //test_exp_image();
  //test_pixelwise_minfo();
  test_exhaustive_minfo();
  
}

TESTMAIN( test_matcher_translation );
