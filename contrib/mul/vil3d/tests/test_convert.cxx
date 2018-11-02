// This is mul/vil3d/tests/test_convert.cxx
#include <iostream>
#include <vxl_config.h> // for vxl_byte
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_convert.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_math.h>
#include <testlib/testlib_test.h>


static void test_convert_diff_types(const char * golden_data_dir)
{
  std::cout << "***********************************************************************\n"
           << " Testing vil3d_convert_cast(vil3d_image_view_base,vil3d_image_view<T>)\n"
           << "***********************************************************************\n";

  std::string datadir = golden_data_dir;
  if (*golden_data_dir) datadir += "/";

  vil3d_image_view<vxl_uint_16> image1 = vil3d_load((datadir + "ff_grey_cross.gipl").c_str());
  vil3d_image_view_base_sptr image_base1 = vil3d_load((datadir + "ff_grey_cross.gipl").c_str());
  TEST("Loading images" , image1 && image_base1, true);

  vil3d_image_view<vxl_uint_32> image_32_1;
  vil3d_convert_cast( image1, image_32_1 );
  TEST("Converting explicitly 16bit to 32bit", image_32_1(4,0,3), vxl_uint_32( image1(4,0,3) ));

  vil3d_image_view<vxl_uint_32> image_32_2 = vil3d_convert_cast(vxl_uint_32(), image_base1 );
  TEST("Converting implicitly 16bit to 32bit", image_32_2(4,0,3), vxl_uint_32( image1(4,0,3) ));
#ifdef DEBUG
  if ( image_32_2 )
    vil3d_print_all(std::cout, image_32_2);
  else
    std::cout << "(no dump)\n";
#endif
}

static void test_convert_stretch_range()
{
  std::cout<<"testing vil3d_convert_stretch_range(src,dest):\n";
  vil3d_image_view<float> f_image(10,10,10);
  for (unsigned k=0;k<f_image.nk();++k)
    for (unsigned j=0;j<f_image.nj();++j)
      for (unsigned i=0;i<f_image.ni();++i)
        f_image(i,j,k)=0.1f*i+0.01f*j+0.001f*k+5.f;

#ifdef DEBUG
  vil3d_print_all(std::cout, f_image) ;
#endif

  vil3d_image_view<vxl_byte> b_image;
  vil3d_convert_stretch_range(f_image,b_image);
  TEST("Width",b_image.ni(),f_image.ni());
  TEST("Height",b_image.nj(),f_image.nj());
  TEST("Depth",b_image.nk(),f_image.nk());
  vxl_byte min_b,max_b;
  vil3d_math_value_range(b_image,min_b,max_b);
  TEST("Min. value",min_b,0);
  TEST("Max. value",max_b,255);
  TEST("b_image(5,5)",b_image(5,5,5),vxl_byte( (5.555-5.0)*255/0.999 ) );
}

static void test_convert_to_n_planes()
{
  constexpr unsigned n = 10;
  std::cout<<"testing vil3d_convert_to_n_planes(src,dest):\n";
  vil3d_image_view<float> f_image(n,n,n,2);
  vil3d_image_view<float> f_image_expected(n,n,n,3);
  vil3d_image_view<vxl_uint_16> u16_image_expected(n,n,n,3);
  for (unsigned k=0;k<f_image.nk();++k)
    for (unsigned j=0;j<f_image.nj();++j)
      for (unsigned i=0;i<f_image.ni();++i)
      {
        f_image(i,j,k,0)=1.0f*i+10.0f*j+100.0f*k+5.0f;
        f_image(i,j,k,1)=1.0f*i+10.0f*j+100.0f*k;
        f_image_expected(i,j,k,0)= i + 10.0f*j + 100.0f*k + 5.0f;
        f_image_expected(i,j,k,1)= i + 10.0f*j + 100.0f*k;
        f_image_expected(i,j,k,2)= i + 10.0f*j + 100.0f*k + 5.0f;
        u16_image_expected(i,j,k,0)= static_cast<vxl_uint_16>(i + 10*j + 100*k + 5);
        u16_image_expected(i,j,k,1)= static_cast<vxl_uint_16>(i + 10*j + 100*k);
        u16_image_expected(i,j,k,2)= static_cast<vxl_uint_16>(i + 10*j + 100*k + 5);
      }

#ifdef DEBUG
  vil3d_print_all(std::cout, f_image);
#endif

  vil3d_image_view_base_sptr f_image_ref = new vil3d_image_view<float>(f_image);

  vil3d_image_view<float> f_image_dest =
    vil3d_convert_to_n_planes(3, f_image_ref);

  TEST("Image as expected",vil3d_image_view_deep_equality(f_image_dest,f_image_expected), true);

  vil3d_math_scale_and_offset_values(f_image,1.0f,0.499f);

  vil3d_image_view_base_sptr f_image_dest_sptr(new vil3d_image_view<float>(f_image_dest));
  vil3d_image_view<vxl_uint_16> image_16_3 = vil3d_convert_round(vxl_uint_16(), f_image_dest_sptr);
  TEST("implicit vil3d_convert_round float to 16bit with rounding" , vil3d_image_view_deep_equality(image_16_3, u16_image_expected), true);

  { // print out values the function will use, because we are getting odd results on some platforms
    std::streamsize oldprec = std::cout.precision(18);
    float fmaxp, fminp;
    vil3d_math_value_range(f_image_dest,fminp,fmaxp);
    double b = 0.0;
    if (fmaxp-fminp >0)
      b = (65535.999)/static_cast<double>(fmaxp-fminp);
    double a = -1.0*fminp*b + 0.0;
    std::cout << "input (minp=" << fminp << ", maxp=" << fmaxp << ')' << std::endl
             << "trans (a=" << a << ", b=" << b << ')' << std::endl;
    std::cout.precision(oldprec);
  }
  vil3d_image_view<vxl_uint_16> image_16_3_stretched = vil3d_convert_stretch_range(vxl_uint_16(), f_image_dest_sptr);
  vxl_uint_16 minp,maxp;
  vil3d_math_value_range(image_16_3_stretched,minp,maxp);
  TEST("implicit vil3d_convert_stretch_range float to 16bit with rounding", minp==0 && maxp==65535, true);
  std::cout << "output (minp=" << minp << ", maxp=" << maxp << ')' << std::endl;
#ifdef DEBUG
  vil3d_print_all(std::cout, image_16_3_stretched);
  vil3d_print_all(std::cout, image_16_3);
  vil3d_print_all(std::cout, u16_image_expected);
#endif
}

static void test_convert(int argc, char* argv[])
{
  test_convert_to_n_planes();
  test_convert_stretch_range();
  test_convert_diff_types(argc>1 ? argv[1] : "file_read_data");
}

TESTMAIN_ARGS(test_convert);
