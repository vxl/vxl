#include <testlib/testlib_test.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_lsqr_cost_func.h>
#include <ihog/ihog_cost_func.h>
#include <ihog/ihog_minimizer.h>
#include <ihog/ihog_transform_2d.h>
#include <vcl_cstdlib.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/vnl_matlab_filewrite.h>
#include <vil/vil_convert.h>
#include <vul/vul_timer.h>


void test_lsqr_min( const vil_image_view<float>& img1,
                    const vil_image_view<float>& img2 )
{
  ihog_world_roi test_roi(255,255,vgl_point_2d<double>(0.0,0.0),
                          vgl_vector_2d<double>(0.95,0.0),
                          vgl_vector_2d<double>(0.0,0.95));
  //ihog_world_roi test_roi(255,255);

  ihog_transform_2d init_xform;
  init_xform.set_rigid_body (1.0,1.0,0.0);

  ihog_image<float> image1(img1, ihog_transform_2d());
  ihog_image<float> image2(img1, ihog_transform_2d());
  ihog_lsqr_cost_func test_cost_func(image1, image2, test_roi, init_xform);

  vnl_matrix<double> result(50,50);
  vnl_vector<double> param(3), fx;
  param[0] = 0.0;
  for (int i=0; i<50; ++i) {
    for (int j=0; j<50; ++j) {
      param[1] = (i-25)/4.0-5;  param[2] = (j-25)/4.0-5;
      vcl_cout << param << "   \t -> ";
      test_cost_func.f(param,fx);
      vcl_cout << fx.rms() << vcl_endl;
      result[i][j] = fx.rms();
    }
  }
  vnl_matlab_filewrite matlab("C:/MATLAB/work/vxl.mat");
  matlab.write(result,"result");

  vnl_matrix<float> f1(img1.top_left_ptr(), img1.ni(), img1.nj());
  vnl_matrix<float> f2(img2.top_left_ptr(), img2.ni(), img2.nj());
  matlab.write(f1, "img1");
  matlab.write(f2, "img2");

#if 0
  vil_image_view<float> test_image;
  param[0] = 10;  param[1] = -10;
  test_cost_func.f(param,fx);
  test_image = test_cost_func.last_xformed_image();
  vnl_matrix<float> test_m(test_image.top_left_ptr(), test_image.ni(), test_image.nj());
  matlab.write(test_m, "test");
#endif // 0
  vnl_levenberg_marquardt test_minimizer(test_cost_func);
  test_minimizer.set_verbose(true);
  test_minimizer.set_trace(true);

  vnl_vector<double> init_params(3);
  init_params[0] = 0.02;
  init_params[1] = -5;
  init_params[2] = -5;

  test_minimizer.minimize(init_params);

  test_minimizer.diagnose_outcome();

  TEST("Levenberg Marquardt",true,true);
}


void test_amoeba_min( const vil_image_view<float>& img1,
                      const vil_image_view<float>& img2 )
{
  ihog_world_roi test_roi(255,255);

  ihog_transform_2d init_xform;
  init_xform.set_translation(1.0,0.0);

  ihog_cost_func test_cost_func(img1, img2, test_roi, init_xform);

  vnl_matrix<double> result(50,50);
  vnl_vector<double> param(2);
#if 0
  for (int i=0; i<50; ++i) {
    for (int j=0; j<50; ++j) {
      param[0] = (i-25)/4.0-5;  param[1] = (j-25)/4.0-5;
      vcl_cout << param << "   \t -> ";
      double cost = test_cost_func.f(param);
      vcl_cout << cost << vcl_endl;
      result[i][j] = cost;
    }
  }
#endif // 0
  vnl_matlab_filewrite matlab("C:/MATLAB/work/vxl.mat");
  matlab.write(result,"result");

  vnl_matrix<float> f1(img1.top_left_ptr(), img1.ni(), img1.nj());
  vnl_matrix<float> f2(img2.top_left_ptr(), img2.ni(), img2.nj());
  matlab.write(f1, "img1");
  matlab.write(f2, "img2");
#if 0
  vil_image_view<float> test_image;
  param[0] = 10;  param[1] = -10;
  test_cost_func.f(param,fx);
  test_image = test_cost_func.last_xformed_image();
  vnl_matrix<float> test_m(test_image.top_left_ptr(), test_image.ni(), test_image.nj());
  matlab.write(test_m, "test");
#endif // 0
  vnl_amoeba test_minimizer(test_cost_func);
  test_minimizer.set_max_iterations(50);

  vnl_vector<double> init_params(2);
  init_params[0] = -5.0;
  init_params[1] = -5.0;

  vul_timer time;
  test_minimizer.minimize(init_params);

  vcl_cout << "result " << init_params << '\n'
           << " in " << test_minimizer.get_num_evaluations() << " iterations"
           << " and " << time.real() << " msec" << vcl_endl;

  TEST("Amoeba",true,true);
}


void test_minimizer( const vil_image_view<float>& img1,
                     const vil_image_view<float>& img2 )
{
  ihog_world_roi test_roi(200,200,vgl_point_2d<double>(20.0,20.0),
                          vgl_vector_2d<double>(0.99,0.0),
                          vgl_vector_2d<double>(0.0,0.99));
  //ihog_world_roi test_roi(255,255);

  ihog_transform_2d init_xform;
  init_xform.set_rigid_body(0.174532, 0.0, 0.0);


  ihog_image<float> image1(img1, ihog_transform_2d());
  ihog_image<float> image2(img1, ihog_transform_2d());
  ihog_minimizer minimizer(image1, image2, test_roi);

  vul_timer time;
  minimizer.minimize(init_xform);

  vcl_cout << "result " << init_xform << '\n'
           << " in " << time.real() << " msec" << vcl_endl;

  TEST("Minimizer",true,true);
}


MAIN( test_minimizer )
{
  START ("Minimizer");

  vil_image_view<vxl_byte> frame1(255,255);
  vil_image_view<vxl_byte> frame2(255,255);
  frame1.fill(255);
  frame2.fill(255);

  unsigned off_j = 10;
  unsigned off_i = 7;
  for (unsigned j=0;j<255+off_j;++j){
    for (unsigned i=0;i<255+off_i;++i){
      unsigned char rand_I = (unsigned char)(vcl_rand());
      char noise= char(vcl_rand())/32;
      //vcl_cout << int(noise) << ", ";
      if (i<255 && j<255)
        frame1(i,j) = rand_I;
      if (i>=off_i && j>=off_j)
        frame2(i-off_i,j-off_j) = rand_I+noise;
    }
  }
  vil_image_view<double> d_frame1, d_frame2;
  vil_convert_stretch_range(frame1, d_frame1, 0.0, 1.0);
  vil_convert_stretch_range(frame2, d_frame2, 0.0, 1.0);

  vil_image_view<float> f_frame1, f_frame2;
  vil_convert_cast(d_frame1, f_frame1);
  vil_convert_cast(d_frame2, f_frame2);

  vil_gauss_filter_5tap(f_frame1,f_frame1,vil_gauss_filter_5tap_params(1));
  vil_gauss_filter_5tap(f_frame2,f_frame2,vil_gauss_filter_5tap_params(1));

  //test_lsqr_min(f_frame1, f_frame2);
  //test_amoeba_min(f_frame1, f_frame2);

  test_minimizer(f_frame1, f_frame2);

  SUMMARY();
}
