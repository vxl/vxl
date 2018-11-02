// This is brl/bseg/brip/tests/test_extrema.cxx

#include <iostream>
#include <iomanip>
#include <vil/vil_image_view.h>
#include <brip/brip_vil_float_ops.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_extrema()
{
  float lambda0 = 2.0f, lambda1 = 1.0f;
  vbl_array_2d<float> kernel45;
  vbl_array_2d<bool> mask45;
  brip_vil_float_ops::extrema_kernel_mask(lambda0, lambda1, 45.0f,
                                          kernel45, mask45);
  unsigned nck45 = kernel45.cols(), nrk45 = kernel45.rows();
#if 0
  unsigned cc = (nck45-1)/2, rc= (nrk45-1)/2;
  for (int k = -3; k<=3; ++k){
    for (int m = -3; m<=3; ++m)
      std::cout << kernel45[rc+k][cc+m] << ' ';
    std::cout << '\n';
  }
#endif
  vbl_array_2d<float> kernel135;
  vbl_array_2d<bool> mask135;
  brip_vil_float_ops::extrema_kernel_mask(lambda0, lambda1, 135.0f,
                                          kernel135, mask135);
  unsigned nck135 = kernel135.cols(), nrk135 = kernel135.rows();
  bool good = nck135==nck45 && nrk45 == nrk135;
  float sum = 0.0f;
  if (good)
    for (unsigned r = 0; r<nrk45; ++r)
      for (unsigned c = 0; c<nck45; ++c)
        sum += kernel45[r][c]-kernel135[c][r];
  good = good && std::fabs(sum)<1e-5;
  TEST("mask kernel ", good, true);

  int ni = 33, nj = 33;
  vil_image_view<float> im(ni,nj);
  im.fill(0.0f);
  im(13,14) = 1.0f;  im(14,14) = 1.0f;   im(15,14) = 1.0f;
  im(14,15) = 1.0f;  im(15,15) = 1.0f;   im(16,15) = 1.0f;
  im(15,16) = 1.0f;  im(16,16) = 1.0f;   im(17,16) = 1.0f;
  im(16,17) = 1.0f;  im(17,17) = 1.0f;   im(18,17) = 1.0f;
  im(17,18) = 1.0f;  im(18,18) = 1.0f;   im(19,18) = 1.0f;

  vil_image_view<float> resd =
    brip_vil_float_ops::extrema(im,lambda0, lambda1, 45.0f, true, true);
  float r45b = resd(16,16,0), m45b = resd(13,16, 1);
  TEST_NEAR("bright response", r45b, 0.46213278, 0.05);
  TEST_NEAR("bright response", m45b, 0.46213278, 0.05);
  im.fill(1.0f);
  im(13,14) = 0.0f;  im(14,14) = 0.0f;   im(15,14) = 0.0f;
  im(14,15) = 0.0f;  im(15,15) = 0.0f;   im(16,15) = 0.0f;
  im(15,16) = 0.0f;  im(16,16) = 0.0f;   im(17,16) = 0.0f;
  im(16,17) = 0.0f;  im(17,17) = 0.0f;   im(18,17) = 0.0f;
  im(17,18) = 0.0f;  im(18,18) = 0.0f;   im(19,18) = 0.0f;

  resd = brip_vil_float_ops::extrema(im,lambda0, lambda1, 45.0f, false, true);
  float r45d = resd(16,16,0), m45d = resd(13,16, 1);
  TEST_NEAR("dark response", r45d, 0.42070714, 0.05);
  TEST_NEAR("dark response", m45d, 0.42070714, 0.05);

  // test inscribed rectangle in response ellipse
  lambda0 = 3.0f;
  float u_rect, v_rect;
  brip_vil_float_ops::max_inscribed_rect(lambda0, lambda1, 45.0f, u_rect, v_rect);
  TEST_NEAR("Inscribed rectangle +45", u_rect, 2.1162584f, 0.1f);

  brip_vil_float_ops::max_inscribed_rect(lambda0, lambda1, -45.0f, u_rect, v_rect);
  TEST_NEAR("Inscribed rectangle -45", u_rect, 2.1162584f, 0.1f);

  brip_vil_float_ops::max_inscribed_rect(lambda0, lambda1, 90.0f, u_rect, v_rect);
  TEST_NEAR("Inscribed rectangle 90", u_rect, 1.9781584f, 0.1f);

  //test fast extrema
  lambda0 = 3.0f;
  lambda1 = 1.5f;
  im.fill(0.0f);
  im(16,16) = 1.0f;
  std::cout << "lambda0 = " << lambda0 << "  lambda1 = " << lambda1 << '\n';
  std::cout.precision(3);

  std::cout << " 2:1 theta = 0\n";
  vil_image_view<float> res =
    brip_vil_float_ops::fast_extrema(im, lambda0, lambda1, 0.0f);
  float max = 0;
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  float rf = max; max = 0;
  std::cout << " 2:1 theta = 0, non-decomposed\n";
  res = brip_vil_float_ops::extrema(im, lambda0, lambda1, 0.0f);
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  float rc = max; max = 0;
  TEST_NEAR("O degrees fast vs. composed", rf, rc, 0.01);

  std::cout << " 2:1 theta = 90\n";
  res = brip_vil_float_ops::fast_extrema(im, lambda0, lambda1, 90.0f);
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  rf = max; max =0;
  std::cout << " 2:1 theta = 90 non-decomposed\n";
  res = brip_vil_float_ops::extrema(im, lambda0, lambda1, 90.0f);
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  rc = max; max = 0;
  TEST_NEAR("9O degrees fast vs. composed", rf, rc, 0.01);

  std::cout << " 2:1 theta = 45\n";
  res = brip_vil_float_ops::fast_extrema(im, lambda0, lambda1, 45.0f);
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  rf = max; max = 0;
  std::cout << " 2:1 theta = 45 non-decomposed\n";
  res = brip_vil_float_ops::extrema(im, lambda0, lambda1, 45.0f);
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  rc = max; max = 0;
  TEST_NEAR("45 degrees fast vs. composed", rf, rc, 0.05);
  std::cout << " 2:1 theta = -45\n";
  res = brip_vil_float_ops::fast_extrema(im, lambda0, lambda1, -45.0f);
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  rf = max; max = 0;
  std::cout << " 2:1 theta = -45 non-decomposed\n";
  res = brip_vil_float_ops::extrema(im, lambda0, lambda1, -45.0f);
  for(unsigned j = 14; j<18; ++j){
    for(unsigned i = 13; i<19; ++i){
      std::cout << std::fixed << res(i,j) << ' ';
      if(res(i,j)>max) max = res(i,j);
    }
    std::cout << '\n';
  }
  rc = max;
  TEST_NEAR("-45 degrees fast vs. composed", rf, rc, 0.01);

  std::cout << " test rotational extrema\n";

  im.fill(0.0f);
  im(13,14) = 1.0f;  im(14,14) = 1.0f;   im(15,14) = 1.0f;
  im(14,15) = 1.0f;  im(15,15) = 1.0f;   im(16,15) = 1.0f;
  im(15,16) = 1.0f;  im(16,16) = 1.0f;   im(17,16) = 1.0f;
  im(16,17) = 1.0f;  im(17,17) = 1.0f;   im(18,17) = 1.0f;
  im(17,18) = 1.0f;  im(18,18) = 1.0f;   im(19,18) = 1.0f;

  for(unsigned j = 13; j<20; ++j){
    for(unsigned i = 12; i<20; ++i){
      std::cout << std::fixed << im(i,j) << ' ';
    }
    std::cout << '\n';
  }
  std::cout << "----------\n";
  lambda0 = 3.0f; lambda1 = 1.0f;
  vil_image_view<float> output = brip_vil_float_ops::extrema_rotational(im, lambda0, lambda1, 15.0f, true);
  for(unsigned j = 13; j<20; ++j){
    for(unsigned i = 12; i<20; ++i){
      std::cout << std::fixed << output(i,j,1) << ' ';
    }
    std::cout << '\n';
  }
  std::cout << "----------\n";
}


TESTMAIN(test_extrema);
