#include <iostream>
#include <cmath>
#include <string>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <bil/algo/bil_detect_ridges.h>

#include <vnl/vnl_math.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_gauss_filter.h>

template <class T>
inline void vil_convert_stretch_range(const vil_image_view<T>& src,
                                      vil_image_view<vxl_uint_16>& dest)
{
  T min_b,max_b;
  vil_math_value_range(src,min_b,max_b);
  double a = -1.0*double(min_b);
  double b = 0.0;
  if (max_b-min_b >0) b = 65536.0/(max_b-min_b);
  dest.set_size(src.ni(), src.nj(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned j = 0; j < src.nj(); ++j)
      for (unsigned i = 0; i < src.ni(); ++i)
         dest(i,j,p) = static_cast<vxl_uint_16>( b*( src(i,j,p)+ a ) );
}

class vil_math_abs_functor
{
 public:
  vxl_byte operator()(vxl_byte x) const { return x; }
  unsigned operator()(unsigned x) const { return x; }
  int operator()(int x)           const { return x<0 ? -x : x; }
  short operator()(short x)       const { return x<0 ? -x : x; }
  float operator()(float x)       const { return std::fabs(x); }
  double operator()(double x)     const { return std::fabs(x); }
};

static void test_bil_detect_ridges(int argc, char* argv[])
{
  assert(argc > 1);
  vil_image_resource_sptr res = vil_load_image_resource((std::string(argv[1]) + "/c.20.tif").c_str());
  TEST("File loading", !res, false);
  vil_image_view<vxl_uint_16> view_uint16 = res->get_view();
  vil_image_view<float> view_float;

  vil_convert_cast(view_uint16,view_float);

  vil_image_view<int> rho;
  vil_image_view<float> ex;
  vil_image_view<float> ey;
  vil_image_view<float> lambda;

  {
    bil_detect_ridges( view_float, 1.5f, 0.7f, rho, ex, ey, lambda);
    TEST("rho", rho(1,1), 0);
    TEST("ey",  ey(1,1),  1);
    TEST("lambda", lambda(1,1), 0);
    vil_image_view<vxl_byte> ridge_byte;
    vil_convert_stretch_range(rho,ridge_byte);
    vil_save(ridge_byte,"other_ridgetest0.tif");

    vil_image_view<vxl_uint_16> lambda_uint;
    vil_convert_stretch_range(lambda,lambda_uint);
    vil_save(lambda_uint,"other_ridgetest1.tif");

    vil_image_view<vxl_uint_16> ex_uint;
    vil_convert_stretch_range(ex,ex_uint);
    vil_save(ex_uint,"other_ridgetest2.tif");

    vil_image_view<vxl_uint_16> ey_uint;
    vil_convert_stretch_range(ey,ey_uint);
    vil_save(ey_uint,"other_ridgetest3.tif");

    vil_image_view<float> rho_dot_lambda;
    vil_transform(lambda,vil_math_abs_functor());
    vil_image_view<float> rho_float;
    vil_convert_cast(rho,rho_float);
    vil_math_image_product(lambda,rho_float,rho_dot_lambda);

    vil_image_view<vxl_uint_16> rho_dot_lambda_uint;
    vil_convert_stretch_range(rho_dot_lambda,rho_dot_lambda_uint);
    vil_save(rho_dot_lambda_uint,"other_ridgetest4.tif");
  }

  {
    vil_image_view<float> ix;
    vil_image_view<float> iy;
    vil_image_view<float> ixx;
    vil_image_view<float> iyy;
    vil_image_view<float> ixy;

    vil_gauss_filter_2d(view_float, view_float, 1.5,vnl_math::rnd(3*1.5/2.));

    vil_sobel_3x3(view_float,ix,iy);
    vil_sobel_3x3(ix,ixx,ixy);
    vil_sobel_3x3(iy,ixy,iyy);

    bil_detect_ridges( ix, iy, ixx, iyy, ixy,
                       1.5f, 0.7f,
                       rho, ex, ey, lambda);

    vil_image_view<vxl_byte> ridge_byte;
    vil_convert_stretch_range(rho,ridge_byte);
    vil_save(ridge_byte,"ridgetest0.tif");

    vil_image_view<vxl_uint_16> lambda_uint;
    vil_convert_stretch_range(lambda,lambda_uint);
    vil_save(lambda_uint,"ridgetest1.tif");

    vil_image_view<vxl_uint_16> ex_uint;
    vil_convert_stretch_range(ex,ex_uint);
    vil_save(ex_uint,"ridgetest2.tif");

    vil_image_view<vxl_uint_16> ey_uint;
    vil_convert_stretch_range(ey,ey_uint);
    vil_save(ey_uint,"ridgetest3.tif");

    vil_image_view<float> rho_dot_lambda;
    vil_transform(lambda,vil_math_abs_functor());
    vil_image_view<float> rho_float;
    vil_convert_cast(rho,rho_float);
    vil_math_image_product(lambda,rho_float,rho_dot_lambda);

    vil_image_view<vxl_uint_16> rho_dot_lambda_uint;
    vil_convert_stretch_range(rho_dot_lambda,rho_dot_lambda_uint);
    vil_save(rho_dot_lambda_uint,"ridgetest4.tif");
  }
}

TESTMAIN_ARGS(test_bil_detect_ridges);
